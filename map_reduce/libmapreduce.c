/** @file libmapreduce.c */
/* 
 * CS 241
 * The University of Illinois
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <pthread.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/wait.h>
#include <poll.h>

#include "libmapreduce.h"
#include "libds/libds.h"


static const int BUFFER_SIZE = 2048;  /**< Size of the buffer used by read_from_fd(). */

pthread_t thread1;

fd_set read_ends;// read end fds

pid_t* pids;// children processes

char** buffers;// buffers for each process

unsigned long revision=0;

int* done;// state of each process

/**
 * Adds the key-value pair to the mapreduce data structure.  This may
 * require a reduce() operation.
 *
 * @param key
 *    The key of the key-value pair.  The key has been malloc()'d by
 *    read_from_fd() and must be free()'d by you at some point.
 * @param value
 *    The value of the key-value pair.  The value has been malloc()'d
 *    by read_from_fd() and must be free()'d by you at some point.
 * @param mr
 *    The pass-through mapreduce data structure (from read_from_fd()).
 */
static void process_key_value(const char *key, const char *value, mapreduce_t *mr)
{
	const char*  temp_value=datastore_get(&mr->d, key, &revision);

	if(temp_value!=NULL)
	{
		const char *  new  =mr->reduce(temp_value,value);
		revision=datastore_update(&mr->d,key,new, revision);

		return;
	}

	revision=datastore_put(&mr->d,key,value);

	return;
}






/**
 * Helper function.  Reads up to BUFFER_SIZE from a file descriptor into a
 * buffer and calls process_key_value() when for each and every key-value
 * pair that is read from the file descriptor.
 *
 * Each key-value must be in a "Key: Value" format, identical to MP1, and
 * each pair must be terminated by a newline ('\n').
 *
 * Each unique file descriptor must have a unique buffer and the buffer
 * must be of size (BUFFER_SIZE + 1).  Therefore, if you have two
 * unique file descriptors, you must have two buffers that each have
 * been malloc()'d to size (BUFFER_SIZE + 1).
 *
 * Note that read_from_fd() makes a read() call and will block if the
 * fd does not have data ready to be read.  This function is complete
 * and does not need to be modified as part of this MP.
 *
 * @param fd
 *    File descriptor to read from.
 * @param buffer
 *    A unique buffer associated with the fd.  This buffer may have
 *    a partial key-value pair between calls to read_from_fd() and
 *    must not be modified outside the context of read_from_fd().
 * @param mr
 *    Pass-through mapreduce_t structure (to process_key_value()).
 *
 * @retval 1
 *    Data was available and was read successfully.
 * @retval 0
 *    The file descriptor fd has been closed, no more data to read.
 * @retval -1
 *    The call to read() produced an error.
 */
static int read_from_fd(int fd, char *buffer, mapreduce_t *mr)
{
	/* Find the end of the string. */
	int offset = strlen(buffer);

	/* Read bytes from the underlying stream. */
	int bytes_read = read(fd, buffer + offset, BUFFER_SIZE - offset);
	if (bytes_read == 0)
		return 0;
	else if(bytes_read < 0)
	{
		fprintf(stderr, "error in read.\n");
		return -1;
	}

	buffer[offset + bytes_read] = '\0';

	/* Loop through each "key: value\n" line from the fd. */
	char *line;
	while ((line = strstr(buffer, "\n")) != NULL)
	{
		*line = '\0';

		/* Find the key/value split. */
		char *split = strstr(buffer, ": ");
		if (split == NULL)
			continue;

		/* Allocate and assign memory */
		char *key = malloc((split - buffer + 1) * sizeof(char));
		char *value = malloc((strlen(split) - 2 + 1) * sizeof(char));

		strncpy(key, buffer, split - buffer);
		key[split - buffer] = '\0';

		strcpy(value, split + 2);

		/* Process the key/value. */
		process_key_value(key, value, mr);

		/* Shift the contents of the buffer to remove the space used by the processed line. */
		memmove(buffer, line + 1, BUFFER_SIZE - ((line + 1) - buffer));
		buffer[BUFFER_SIZE - ((line + 1) - buffer)] = '\0';
	}

	return 1;
}

void* worker(void* arg)
{

	mapreduce_t* mr= (mapreduce_t*)arg;// cast to mr

	//char** temp_data=mr->data;// the data set 
	int length=mr->len;

    buffers=malloc(length*sizeof(char*));

	int i;
    for(i=0;i<length;i++)
    {
        buffers[i]=malloc(BUFFER_SIZE+1);    
        buffers[i][0]='\0';//init
    }


    int counter=0;
    while(counter<length)
    {
		FD_ZERO(&read_ends);
        for(i=0;i<length;i++)    
            FD_SET(mr->fds[i][0],&read_ends);// set read fds in the set
        
		select(FD_SETSIZE,&read_ends,NULL,NULL,NULL);

		for(i=0;i<length;i++)
        {

			int isset = FD_ISSET(mr->fds[i][0], &read_ends );// check if the fd is ready
			if(isset)
			{

				int read=read_from_fd(mr->fds[i][0],buffers[i],mr);
				if(read==0 && done[i]==0)
				{
					counter++;
				
					FD_CLR(mr->fds[i][0],&read_ends);
									
					done[i]=1;
	
				}
			}
		}    

    }

}

/**
 * Initialize the mapreduce data structure, given a map and a reduce
 * function pointer.
 */
void mapreduce_init(mapreduce_t *mr, 
                    void (*mymap)(int, const char *), 
                    const char *(*myreduce)(const char *, const char *))
{
	mr->map=mymap;
	mr->reduce=myreduce;

	datastore_init(&mr->d);// data store init

	mr->data=NULL;

	mr->len=0;

	mr->fds=NULL;		
	

}



/**
 * Starts the map() processes for each value in the values array.
 * (See the MP description for full details.)
 */
void mapreduce_map_all(mapreduce_t *mr, const char **values)
{
	mr->data=values;

	int length=0;
	while(values[length]!=NULL)
		length++;
	// get the length of the data
	mr->len=length;

	done=malloc(length*sizeof(int));// allocate space for state for each process

	int i;
	for(i=0;i<length;i++)
		done[i]=0;// the state of each process
		
	mr->fds=malloc(length*sizeof(int*));// one process per data set

	for(i=0;i<length;i++)
		mr->fds[i]=malloc(2*sizeof(int));// allocate read and write fds for each process

    pids=malloc(sizeof(int)*length);
	for(i=0;i<length;i++)
	{
		pipe(mr->fds[i]);// set up pipes

		pids[i]=fork();
		if(pids[i]==0)// child
		{
            close(mr->fds[i][0]);//close read end
			mr->map(mr->fds[i][1],values[i]);// map closes read write end in the end
			exit(0);//exit child process
		}
		else
		{
			close(mr->fds[i][1]);// parent. No other can write 
		}
		
	}

	pthread_create(&thread1,NULL,worker,mr);// create a worker thread
	
	
}


/**
 * Blocks until all the reduce() operations have been completed.
 * (See the MP description for full details.)
 */
void mapreduce_reduce_all(mapreduce_t *mr)
{
	int status;
	int i;
	for(i=0;i<mr->len;i++)
		waitpid(pids[i],&status,0);// wait for child process

	pthread_join(thread1,NULL);
}


/**
 * Gets the current value for a key.
 * (See the MP description for full details.)
 */
const char *mapreduce_get_value(mapreduce_t *mr, const char *result_key)
{
	return datastore_get(&mr->d,result_key,&revision);
}


/**
 * Destroys the mapreduce data structure.
 */
void mapreduce_destroy(mapreduce_t *mr)
{
	
	int i;

	free(pids);
	free(done);

	//if(mr!=NULL)

	for(i=0;i<mr->len;i++)
	{
		free(mr->fds[i]);
		free(buffers[i]);
		//free(mr->data[i]);
	}	

	//free(mr->data);
	free(mr->fds);
	free(buffers);


	
	datastore_destroy(&mr->d);// free the datastore

}
