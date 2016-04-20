/** @file log.c */
#include <stdlib.h>
#include <string.h>
#include "log.h"

/**
 * Initializes the log.
 *
 * You may assuem that:
 * - This function will only be called once per instance of log_t.
 * - This function will be the first function called per instance of log_t.
 * - All pointers will be valid, non-NULL pointer.
 *
 * @param l
 *   Pointer to the log data structure to be initialized.
 */
void log_init(log_t* l)
{
	//l->tail=malloc(sizeof(node));
	l->head=NULL;
	l->tail=NULL;
	l->logSize=0;
}

/**
 * Frees all internal memory associated with the log.
 *
 * You may assume that:
 * - This function will be called once per instance of log_t.
 * - This funciton will be the last function called per instance of log_t.
 * - All pointers will be valid, non-NULL pointer.
 *
 * @param l
 *    Pointer to the log data structure to be destoryed.
 */
void log_destroy(log_t* l)
{
	node* temp=l->head;
	node* temp2=temp;
	while(temp!=NULL)
	{
		//free(temp->cmd);
		temp=temp->next;
		//free(temp2->cmd);
		free(temp2);
		temp2=temp;
	}
	l->logSize=0;
	free(l);
	
}

/**
 * Appends an item to the end of the log.
 *
 * The item MUST NOT be copied.  Only a pointer is stored in the log.
 *
 * You may assume that:
* - All pointers will be valid, non-NULL pointer.
*
 * @param l
 *    Pointer to the log data structure.
 * @param item
 *    Pointer to a string to be added to the log.
 */
void log_append(log_t* l, char *item)
{
	node* temp=l->tail;
	//there is no node in the log:
	if(temp==NULL)
	{
		temp=malloc(sizeof(node));
		l->head=temp;
		l->tail=temp;
		temp->cmd=item;
		temp->prev=NULL;
		temp->next=NULL;
		l->logSize=1;
		return;
	}


	//there is more than 0 node in the node
	temp->next=malloc(sizeof(node));
	node* temp2=temp->next;
	temp2->cmd=item;
	temp2->prev=temp;
	temp2->next=NULL;
	l->tail=temp2;
	l->logSize++;
}

/**
 * Removes and returns the last item in the log.
 *
 * If this function was called following a call to log_append(), the return
 * value will be the value that was just to the log.  If multiple calls are
 * made to log_pop(), is should continue to remove entries from the log in
 * a Last-In First-Out (LIFO) or "stack" order.
 *
 * You may assume that:
* - All pointers will be valid, non-NULL pointer.
*
 * @param l
 *    Pointer to the log data structure.
 *
 * @returns
 *    The last item in the log, or NULL if the log is empty.
 *
 */
char *log_pop(log_t* l)
{
	node* temp=l->tail;
	if(temp==NULL)
		return NULL;
	
	//only one element in the log:
	if(temp->prev==NULL)
	{
		l->head=NULL;
		l->tail=NULL;
		l->logSize=0;
		char* ret=temp->cmd;
		free(temp);
		return ret;
	}

	// if there are more than one elements in the log:
	node* temp2 =temp->prev;
	l->tail=temp2;
	temp2->next=NULL;
	temp->next=NULL;
	temp->prev=NULL;
	free(temp);
	l->logSize--;

    return temp2->cmd;
}

/**
 * Returns a pointer to the idx-th element in the log, where the
 * 0-th element is the first element appended to the log.
 *
 * You may assume that:
 * - All pointers will be valid, non-NULL pointer.
 *
 * @param l
 *    Pointer to the log data structure.
 * @param idx
 *    Zero-based index into the log, where the 0-th entry corresponds to the
 *    first (oldest) entry in the log and the (n-1)-th entry corresponds to
 *    the latest (newest) entry in the log.
 *
 * @returns
 *    The idx-th entry in the log, or NULL if such an extry does not exist.
 */
char *log_at(log_t* l, unsigned int idx) 
{
	if((int)idx>=l->logSize || l->logSize==0)
		return NULL;
	
	node* temp=l->tail;
	while(idx>0)
	{
		temp=temp->prev;
		idx--;
	}
    return temp->cmd;
}

/**
 * Returns the number of elements in the log.
 *
 * You may assume that:
 * - All pointers will be valid, non-NULL pointer.
 *
 * @param l
 *    Pointer to the log data structure.
 *
 * @returns
 *    Number of entires in the log.
 */
unsigned int log_size(log_t* l)
{
    return l->logSize;
}

/**
 * Preforms a newest-to-oldest search of log entries for an entry matching a
 * given prefix.
 *
 * This search starts with the newest or (n-1)-th entry in the log and
 * compares each entry to determine if query is a prefix of the log entry.
 * Upon reaching a match, a pointer to that element is returned.  If no match
 * is found, a NULL pointer is returned.
 *
 * For example, a log may be built with five entries:
 * @code
 *    log_append(&l, "ab  1");
 *    log_append(&l, "a   2");
 *    log_append(&l, "abc 3");
 *    log_append(&l, "ab  4");
 *    log_append(&l, "a   5");
 * @endcode
 *
 * Using the log that was build above:
 * - A call to <tt>log_search(&l, "ab")</tt> will return the pointer to the string "ab  4"
 *   since the search must begin from the newest entry and move backwards.
 * - A call to <tt>log_search(&l, "a")</tt> will return the pointer to the string "a   5".
 * - A call to <tt>log_search(&l, "abc")</tt> will return the pointer to the string "abc 3".
 * - A call to <tt>log_search(&l, "d")</tt> will return a NULL pointer.
 *
 * You may assume that:
 * - All pointers will be valid, non-NULL pointer.
 *
 * @param l
 *    Pointer to the log data structure.
 * @param prefix
 *    The prefix to test each entry in the log for a match.
 *
 * @returns
 *    The newest entry in the log whose string matches the specified prefix.
 *    If no strings has the specified prefix, NULL is returned.
 */
char *log_search(log_t* l, const char *prefix)
{
	if(l->logSize==0)
    	return NULL;

	node* temp=l->head;
	//node* temp2=head->next;
	while(temp!=NULL)
	{
		if( !strncmp((temp->cmd),(prefix),strlen(prefix))  )
			return temp->cmd;
		temp=temp->next;
	}

	return NULL;
}



