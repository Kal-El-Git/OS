/** @file alloc.c */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "debug.h"


//size_t total=0;
//size_t position=0; // last node's position 
char flag= 't';
//void* tail;
//void* begin;
void* free_list=NULL;
//void coalesce(void* ptr);
//structures:


/*
typedef struct _m_data{
	char state;
	void* next;
	void* prev;
	size_t size;// the power of the number 

} m_data;
*/

/**
 * Allocate space for array in memory
 * 
 * Allocates a block of memory for an array of num elements, each of them size
 * bytes long, and initializes all its bits to zero. The effective result is
 * the allocation of an zero-initialized memory block of (num * size) bytes.
 * 
 * @param num
 *    Number of elements to be allocated.
 * @param size
 *    Size of elements.
 *
 * @return
 *    A pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory, a
 *    NULL pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/calloc/
 */
void *calloc(size_t num, size_t size)
{
	/* Note: This function is complete. You do not need to modify it. */
	void *ptr = malloc(num * size);
	
	if (ptr)
		memset(ptr, 0x00, num * size);

	return ptr;
}


/**
 * Allocate memory block
 *
 * Allocates a block of size bytes of memory, returning a pointer to the
 * beginning of the block.  The content of the newly allocated block of
 * memory is not initialized, remaining with indeterminate values.
 *
 * @param size
 *    Size of the memory block, in bytes.
 *
 * @return
 *    On success, a pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a null pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/malloc/
 */
void *malloc(size_t size)
{
//DPRINTF("line87 malloc size:%zu\n",size);
		
	if(flag=='t')
	{
	flag='f';
	//begin=sbrk(0);// start of the heap
	void* start=sbrk(size+16);
	//free_list= (size_t*)start;
	//size_t* curSize=(size_t*)start;
	//curSize++;
	//*curSize=NULL;
	//start=start+16;
	size_t* t=((size_t*)start);
	*t=size;
	//size_t ** t1=((size_t**)(start+8));
	//*t1	=NULL;
	*((size_t**)(start+8))=NULL;
	return (start+16);
	}	
		//DPRINTF("size:%zu\n",size);
	//	DPRINTF("size_t:%zu\n",sizeof(size_t));
	//	DPRINTF("size_t*:%zu\n",sizeof(size_t*));

if(free_list!=NULL){
	void* prev=free_list;
	void* listCur=prev;

	
	size_t temp= (*((size_t*)listCur));
//DPRINTF("line113     %zu\n",temp);
	if(temp>=size){
		
		free_list=*((size_t**)(free_list+8));
		*((size_t**)(listCur+8))=NULL;
		return (listCur+16);
	}

//DPRINTF("line123   temp  %zu\n",temp);
	//prev=listCur;
	listCur=*((size_t**)(listCur+8));
	while(listCur!=NULL){
		size_t temp2=*((size_t*)listCur);
//DPRINTF("line124%zu\n",*((size_t*)(listCur)));
		//DPRINTF("line29 temp2:%zu\n",(temp2));
		if(temp2>size){
			//DPRINTF("line128%zu\n",*((size_t*)(prev)));
		//	DPRINTF("line129 cursize:%zu\n",*((size_t*)(listCur)));
		//	DPRINTF("line132 size:%zu\n",(size));
			*((size_t**)(prev+8))=*((size_t**)(listCur+8));
			*((size_t**)(listCur+8))=NULL;
			return (listCur+16);
		}
		prev=listCur;
		listCur=*((size_t**)(listCur+8));
	}

}
	void* start=sbrk(size+16);

/*	size_t* t=((size_t*)start);
	*t=size;
	size_t ** t1=((size_t**)(start+8));
	*t1	=NULL;
*/
	
	*((size_t*)start)=size;
	*((size_t**)(start+8))=NULL;

	return (start+16);
		
}


	



/**
 * Deallocate space in memory
 * 
 * A block of memory previously allocated using a call to malloc(),
 * calloc() or realloc() is deallocated, making it available again for
 * further allocations.
 *
 * Notice that this function leaves the value of ptr unchanged, hence
 * it still points to the same (now invalid) location, and not to the
 * null pointer.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(),
 *    calloc() or realloc() to be deallocated.  If a null pointer is
 *    passed as argument, no action occurs.
 */
void free(void *ptr)
{


//DPRINTF("line177 free size:%zu\n",*((size_t*)(ptr-16)));
/*
	//free list

	size_t* listPrev=free_list;
	size_t* listCur=free_list;

	//current node
	void* temp=ptr;
	temp=ptr-2*sizeof(size_t);
	size_t* curSize=(size_t*)temp;
	size_t* nextP=tempCur+1;
	size_t* cur=*nextP;


	while(cur!=NULL)
	{
		if((*curSize)<size)//insert 
		{
			cur=listCur;
			listPrev=temp;//change this!!!
			return;
		}

		listPrev=listCur;// change this!!!
		size_t* tempNext=listCur+1;
		listPrev=*tempNext;


	}
*/
	//DPRINTF("line198\n");
	size_t s=*((size_t*)(ptr-16));
    if(free_list==NULL){
		free_list=(ptr-16);	
		*((size_t**)(free_list+8))=NULL;
		return;	
	}
	void* prev=free_list;
	void* curr=(void*)(*((size_t**)(free_list+8)));

	while(curr!=NULL){
			//	DPRINTF("line219%zu\n",*((size_t*)(curr)));
	if(s<=*((size_t*)(curr))){


			((*((size_t**)(prev+8))))=(size_t*)(ptr-16);
/*
			void* temp2=(void*)(*((size_t**)(ptr-8)));
			temp2=curr;*/
			*((size_t**)(ptr-8))=(size_t*)curr;
		return;
	}
	prev=curr;
	curr=*((size_t**)(curr+8));
	}

	//size_t* temp2=(*((size_t**)(prev+8)));
	(*((size_t**)(prev+8)))=(size_t*)(ptr-16);
	*((size_t**)(ptr-8))=NULL;
	//((void*)(*((size_t**)(prev+8))))=ptr-16;
	//*((size_t**)(ptr-8))=NULL;
	//*((size_t**)(prev+8))
	return;

}



/**
 * Reallocate memory block
 *
 * The size of the memory block pointed to by the ptr parameter is changed
 * to the size bytes, expanding or reducing the amount of memory available
 * in the block.
 *
 * The function may move the memory block to a new location, in which case
 * the new location is returned. The content of the memory block is preserved
 * up to the lesser of the new and old sizes, even if the block is moved. If
 * the new size is larger, the value of the newly allocated portion is
 * indeterminate.
 *
 * In case that ptr is NULL, the function behaves exactly as malloc, assigning
 * a new block of size bytes and returning a pointer to the beginning of it.
 *
 * In case that the size is 0, the memory previously allocated in ptr is
 * deallocated as if a call to free was made, and a NULL pointer is returned.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(), calloc()
 *    or realloc() to be reallocated.
 *
 *    If this is NULL, a new block is allocated and a pointer to it is
 *    returned by the function.
 *
 * @param size
 *    New size for the memory block, in bytes.
 *
 *    If it is 0 and ptr points to an existing block of memory, the memory
 *    block pointed by ptr is deallocated and a NULL pointer is returned.
 *
 * @return
 *    A pointer to the reallocated memory block, which may be either the
 *    same as the ptr argument or a new location.
 *
 *    The type of this pointer is void*, which can be cast to the desired
 *    type of data pointer in order to be dereferenceable.
 *    
 *    If the function failed to allocate the requested block of memory,
 *    a NULL pointer is returned, and the memory block pointed to by
 *    argument ptr is left unchanged.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/realloc/
 */
void *realloc(void *ptr, size_t size)
{
	 // "In case that ptr is NULL, the function behaves exactly as malloc()"
	if (!ptr)
		return malloc(size);
	 // "In case that the size is 0, the memory previously allocated in ptr
	 //  is deallocated as if a call to free() was made, and a NULL pointer
	 //  is returned."
	if (!size)
	{
		free(ptr);
		return NULL;
	}

	size_t t= *((size_t*)(ptr-16));
	if(t>=size) return ptr;
	void* newt=malloc(size);

	memcpy(newt,ptr,t);
	free(ptr);
	return newt;


}

