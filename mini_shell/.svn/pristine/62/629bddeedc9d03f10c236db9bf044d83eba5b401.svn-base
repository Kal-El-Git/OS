/** @file testlog.c */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"

int main()
{
	log_t * newlog=(log_t*)malloc(sizeof(log_t));
	log_init(newlog);

	
	
	log_append(newlog,"first ");
	log_append(newlog,"second");
	log_append(newlog,"third");
	log_append(newlog,"fourth");
	log_append(newlog,"fifth");
	log_append(newlog,"sixth");
	unsigned int size=log_size(newlog);

	log_search(newlog,"t");
	printf("the size of the log is %u\n",size);
	log_pop(newlog);
	//log_destroy(newlog);
	size=log_size(newlog);
	printf("the size of the log is %u\n",size);

	log_pop(newlog);
	//log_destroy(newlog);
	size=log_size(newlog);
	printf("the size of the log is %u\n",size);


	log_destroy(newlog);
    return 0;
}
