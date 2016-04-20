#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "log.h"
#include <sys/types.h>
#include <wait.h>
int status;		

int main()
{
		
		size_t size;//command size
		ssize_t cmd_len;//command length
		char cwd[512];//current working directory
		char* line=NULL;		


		//log:
		log_t* log=(log_t*)malloc(sizeof(log_t));
		log_init(log);

		getcwd(cwd,512);// current working directory
		pid_t pid=getpid();// current process id				

		while(1)
		{
				
			//printf("Command executed by pid=%d\n", pid);		

			line= NULL;// command that users input
	
			printf("(pid=%d)%s$ ", pid, cwd);//print the current pid and the current working directory
			//fflush(stdout);//flush the buffer
			cmd_len=getline(&line,&size,stdin);// return the character read, store input in line and input size in size 

			if(cmd_len<0)//failure
				return 0;

			line[cmd_len-1]='\0';// the newline character is turned into null




			if(!strncmp(line,"!",1)  && strncmp(line,"!#",2)!=0 ){
				char* query=log_search(log,line+1);												//	BUILT-IN !query
				if(query==NULL)
					printf("No Match\n");
				else
				{
					log_append(log,query);
					printf("%s matches %s\n", line, query);
					line=query;			
					
				}
				continue;

			}//else

			if(!strcmp(line,"!#")) //&& (log_size(log)!=0))										//BUILT-IN !#
			{
				node* temp=log->tail;
				while(temp!=NULL)
				{
					printf("%s\n",temp->cmd);
					temp=temp->prev;
				}
				printf("Command executed by pid=%d\n", pid);	
				continue;
			}
		


			if(!strncmp(line,"cd ",3))																	//BUILT-IN   cd 
			{
				//log_append(log,line);
				if(chdir(line+3)<0)//error
					printf("%s: No such file or directory\n", line+3);
				else
				{
					if(getcwd(cwd,512)!=NULL)
					{	chdir(line+3);
						log_append(log,line);
						printf("Command executed by pid=%d\n", pid);
					}
				}
					
				continue;
			}//if

			if(!strcmp(line,"exit"))																	//BUILT-IN exit
			{
				log_append(log,"exit");
				printf("Command executed by pid=%d\n", pid);
				break;
			}
		
			
			//other built-in commands	
				
			pid_t pid=fork();
			if(pid==0)//child process
			{	
				char* lineTemp=malloc(strlen(line)*sizeof(char));
				log_append(log,line);
				memcpy(lineTemp,line,strlen(line));
				char* backup[256];
				int n=0;
				backup[0]=strtok(lineTemp," ");
				while(backup[n]!=NULL){
					++n;
					backup[n]=strtok(NULL," ");
				}
				execvp(backup[0],backup);
				free(lineTemp);
				exit(1);
			}
						
			else//parent wait for child process
			{				
				wait(&status);
			}
			
			//system(line);
			
			//free(argv);
			free(line);// free command
		}

	return 0;
}
