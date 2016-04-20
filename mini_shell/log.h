/** @file log.h */

#ifndef __LOG_H_
#define __LOG_H_

/** The log data structure. */
typedef struct _node
{
char* cmd;
struct _node* next;
struct _node* prev;
} node;


typedef struct _log_t
{
node* head;// the bottom of the queue,  
node* tail;// top of the queue
	int logSize;
} log_t;

void log_init(log_t* l);
void log_destroy(log_t* l);

void log_append(log_t* l, char *item);
char *log_pop(log_t* l);
char *log_at(log_t* l, unsigned int idx);
unsigned int log_size(log_t* l);
char *log_search(log_t* l, const char *prefix);


#endif
