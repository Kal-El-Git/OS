/** @file server.c */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <queue.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "queue.h"
#include "libhttp.h"
#include "libdictionary.h"

const char *HTTP_404_CONTENT = "<html><head><title>404 Not Found</title></head><body><h1>404 Not Found</h1>The requested resource could not be found but may be available again in the future.<div style=\"color: #eeeeee; font-size: 8pt;\">Actually, it probably won't ever be available unless this is showing up because of a bug in your program. :(</div></html>";
const char *HTTP_501_CONTENT = "<html><head><title>501 Not Implemented</title></head><body><h1>501 Not Implemented</h1>The server either does not recognise the request method, or it lacks the ability to fulfill the request.</body></html>";

const char *HTTP_200_STRING = "OK";
const char *HTTP_404_STRING = "Not Found";
const char *HTTP_501_STRING = "Not Implemented";

int num=0;
pthread_t threads[256]; 
int fds[256];

void sig_handler(int sig)
{
	puts("handler");
	printf("%d\n",num);
	if(sig==SIGINT)
	{	
		puts("here");
		int i;
		for(i=0;i<num;i++)
		{
			printf("%d ",i);
			shutdown(fds[i],2);	
		}
		printf("\n");
		for(i=0;i<num;i++)
		{
			printf("%d ",i);
			pthread_join(threads[i],NULL);
		}
		printf("\n");
			
	}
	
}
/**
 * Processes the request line of the HTTP header.
 * 
 * @param request The request line of the HTTP header.  This should be
 *                the first line of an HTTP request header and must
 *                NOT include the HTTP line terminator ("\r\n").
 *
 * @return The filename of the requested document or NULL if the
 *         request is not supported by the server.  If a filename
 *         is returned, the string must be free'd by a call to free().
 */
char* process_http_header_request(const char *request)
{
	// Ensure our request type is correct...
	if (strncmp(request, "GET ", 4) != 0)
		return NULL;

	// Ensure the function was called properly...
	assert( strstr(request, "\r") == NULL );
	assert( strstr(request, "\n") == NULL );

	// Find the length, minus "GET "(4) and " HTTP/1.1"(9)...
	int len = strlen(request) - 4 - 9;

	// Copy the filename portion to our new string...
	char *filename = malloc(len + 1);
	strncpy(filename, request + 4, len);
	filename[len] = '\0';

	// Prevent a directory attack...
	//  (You don't want someone to go to http://server:1234/../server.c to view your source code.)
	if (strstr(filename, ".."))
	{
		free(filename);
		return NULL;
	}

	return filename;
}

void* worker(void* arg)
{
	int* sockfd= (int*)arg;//the socket fd
	int fd =*sockfd;
	//printf("fd: %d\n",fd);
	int keep_alive=0;
	
while(1){
// task 3a using libhttp:
	http_t *  h = malloc(sizeof(http_t));
	int status = http_read(h,fd);
	if(status == -1)
	{
		exit(1);
	}

//Task 3b
	const char* request = http_get_status(h);
	printf("request: %s\n",request);

	char* filename= process_http_header_request(request);

	printf("filename:%s\n",filename);

	// body length:
	int content_len;

	//const char* body = http_get_body(h,&content_len);
	//printf("content_len: %d\n",content_len);

//task 3c, 3d
	int response_code;//response code

	const char* http_string;
	const char* http_content;

	char* content_type=malloc(16);
	char* response_file=malloc(256);

	FILE* f;
	//long fileSize;
	char* fileBody=NULL;
	//int fflag=0;
	//printf("response_file: %d,  filename:%s\n", response_file, filename);
	printf("right here?\n");
	if(filename==NULL)//return 501 error
	{
		printf("filename null.  filename:%s\n",filename);
		response_code=501;
		http_content = HTTP_501_CONTENT;
		http_string  = HTTP_501_STRING;
		content_type="text/html";
		content_len=(int)strlen(HTTP_501_CONTENT);
		//fflag=0;
		fileBody=HTTP_501_CONTENT;
	}
	else
	{
		printf("how about here?\n");
		
		if(strcmp(filename,"/")==0)
		{
			//printf("how about here?2\n");
			if((f=fopen("web/index.html","r"))==NULL)//404 
			{
			
			printf("  /  404\n");
			response_code=404;
			http_content=HTTP_404_CONTENT;
			http_string=HTTP_404_STRING;
			content_type="text/html";
			content_len=(int)strlen(HTTP_404_CONTENT);
			fileBody=HTTP_404_CONTENT;

			}
			else{	// 200
			printf("/ 200\n");
			// process as /index.html
			response_code=200;
			//http_content = HTTP_200_CONTENT;
			http_string  = HTTP_200_STRING;
			//*response_file = "web/index.html";
			content_type = "text/html";
			//content_len=(int)strlen(HTTP_200_CONTENT);
			strcpy(response_file,"web/index.html");
			printf("%s\n",response_file);
			
			fseek(f,0,SEEK_END);
			printf("how about here?\n");
			content_len=ftell(f);
			printf("content_len:%d\n", content_len);
			fseek(f,0,SEEK_SET);

			printf("right before filebody?\n");
			fileBody=calloc(content_len+1,1);
			fread(fileBody,content_len,1,f);
			
			//fclose(f);

			}
		}

		else{// not a '/'
			response_file=strcpy(response_file,"web/");
			response_file=strcat(response_file,filename);
			//printf("how about here?print?\n");
			if((f=fopen(response_file,"r"))==NULL){// does not exist
			// process a 404 error
			printf("file 404\n");
			response_code=404;
			http_content = HTTP_404_CONTENT; 
			http_string  = HTTP_404_STRING;
			content_type="text/html";
			content_len=strlen(HTTP_404_CONTENT);
			printf("the file does not exist\n");
			fileBody=HTTP_404_CONTENT;
			}
			else// other files on local :  200
			{

			
			fseek(f,0, SEEK_END);
			printf("file 200");
			content_len=ftell(f);
			printf("content_len:%d\n", content_len);
			fseek(f,0,SEEK_SET);


			fileBody= calloc(content_len+1,1);
			fread(fileBody, content_len,1,f);

			fclose(f);

			response_code=200;
			http_string  = HTTP_200_STRING;
			//strcpy(response_file,filename);			
			//strcat("web/",response_file);

			//content_len=fileSize;

			if( strstr(filename, ".html") )
				content_type="text/html";
			else if(strstr(filename,".css"))
				content_type="text/css";
			else if(strstr(filename,".jpg"))
				content_type="image/jpeg";
			else if(strstr(filename,".png"))
				content_type="image/png";
			else
				content_type="text/plain";
		
		
			}
		}
	}

//task 3e
	char* response=malloc(1024*sizeof(char));

	// code and response string
	sprintf(response, "HTTP/1.1 %d %s\r\n", response_code, http_string);
	printf("3e 2 \nresponse: %s \n",response);

	//content-type
	sprintf(response, "%sContent-Type: %s\r\n",response, content_type);
	//printf("3e 3 \nresponse: %s \n",response);

	//content-length
	sprintf(response, "%sContent-Length: %d\r\n",response, content_len);
	//printf("3e 4\nresponse: %s \n",response);
	//connection:

	
	const char* con =malloc(16*sizeof(char));
	con="Connection";

	const char* connect = dictionary_get(&h->header , con);
	if(connect==NULL)
		{
		sprintf(response, "%sConnection: %s\r\n",response, "close");
		keep_alive=0;
		}
	else{
		if(  strcasecmp(connect,"Keep-Alive")==0 )
		{
			sprintf(response, "%sConnection: %s\r\n", response,"Keep-Alive");
			keep_alive=1;
		}
		else 
		{
			sprintf(response, "%sConnection: %s\r\n", response, "close");
			keep_alive=0;
		}
		}
		
		//empty line
			sprintf(response, "%s\r\n",response);
	
		//sprintf(response, "%s\r\n%s",response, fileBody);
		printf("response: %sbody:%s",response,fileBody);
		//printf("content: %s \n",fileBody);
		//content_len=(int)strlen(response);

	//content or body of the packet:
	//	sprintf(response,"")		


	printf("response: %s \n",response);

	send(fd, response, strlen(response), 0);
	send(fd, fileBody, content_len, 0);
	/*if(fflag)
		send(fd,fileBody,fileSize,0);
	printf("response: %s, strlen: %d\n", response, strlen(response));
	//printf("3e  6\nresponse: %s \n",response);
	*/

	//if(strcasecmp(connect,"Keep-Alive")!=0)
	if(keep_alive==0)
	{
		return NULL;			
	}


}
	

	return ;
}


int main(int argc, char **argv)
{
	//register a signal handler:
	signal(SIGINT, sig_handler);

// task 1:
	int portNum= atoi(argv[1]);// port number 
	printf("the portNum:%d\n",portNum);
	struct sockaddr_in serv_addr, cli_addr;
	int sockfd=socket(AF_INET,SOCK_STREAM,0);// create a socket

	if(sockfd<0)
	{
		//error:
		perror("error in creating the socket: ");
		exit(1);
	}
	//attributes I don't know:
	serv_addr.sin_family=AF_INET;// IPv4
	serv_addr.sin_addr.s_addr=INADDR_ANY;
	serv_addr.sin_port=htons(portNum);
	//bind the host address
	if(bind(sockfd,(struct  sockaddr*)&serv_addr, sizeof(serv_addr)) < 0  )	
	{
		perror("error in binding: ");
		exit(1);
	}

	//now start listening:
	listen(sockfd,10);//backlog 10
	int clilen=sizeof(cli_addr);

// task 2:
	int connection=0;// connection number
	
	while(1)
	{
		//int newsockfd=accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
		fds[connection]=accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
		if(fds[connection]<0)
		{
			perror("error on accept: ");
			exit(1);
		}
		//one accept, one more connection. create threads:
		pthread_create(&threads[connection], NULL, worker, (void*)&fds[connection]); 
		printf("pthread[%d]\n",connection);
		connection++;//increment connection number
		num=connection;
	}//while

	
	close(sockfd);
	return;
}
