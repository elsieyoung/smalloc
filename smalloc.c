#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "smalloc.h"

void *mem;
struct block *freelist;
struct block *allocated_list;

void *smalloc(unsigned int size) {
    struct block *free_node = NULL, *new_allocated = NULL, 
		  *previous = NULL, *tmp = NULL;
    void * address = NULL;
	
    // Search the freelist for a block that is at least size bytes in size
    // Set free_node at the beginning of the freelist.
    free_node = freelist;
	
    // Traverse the freelist till we reach the end or we find a block whose
    // size >= required size.
    while (free_node != NULL && address == NULL){
	// Case 1: find a block of exactly the required size.
	if ((*free_node).size == size){
	    address = (void *)(free_node->addr);
	    // Modify the freelist by deleting the node that has been allocated.
	    // Traverse the freelist to set the pointer at the desired node.
	    tmp = freelist;
	    while (tmp != NULL){
	        if((*tmp).addr != address){
		    previous = tmp;
		    tmp = tmp->next;
	        }
		else{
		    break;
		}
            }
					
	    // Once we find the node, we can delete it.
	    if (tmp == freelist){
		freelist = tmp->next;
	    }
	    else{
	        previous->next = tmp->next;
	    }
	
	    // We insert the node we just delete at the front of the allocated_list.
	    tmp->next = allocated_list;
	    allocated_list = tmp;
        }
		
	// Case 2: find a block that is larger than the required size.
	else if ((*free_node).size > size){
	    address = (void *)((*free_node).addr);
	    // If allocated_list is NULL, allocate a new block of memory for it.
	    if(allocated_list == NULL){
		allocated_list = (struct block*)malloc(sizeof(struct block));
		allocated_list->addr = address;
	        allocated_list->size = size;
		allocated_list->next = NULL;
	    }
				
	    // If allocated_list is not NULL, add a new node in front of the allocated_list.
	    else{
		new_allocated = (struct block*)malloc(sizeof(struct block)); 
		new_allocated->addr = address;
		new_allocated->size = size;
		new_allocated->next = allocated_list;
		allocated_list = new_allocated;
	    }
				
	    // Modify the freelist by spliting the node into two parts.
	    free_node->size = (*free_node).size - size;
	    free_node->addr = (*free_node).addr + size;
	}
		
	// Move the pointer to free_node->next.
	free_node = free_node->next;
    }

    return address;
}


int sfree(void *addr) {
    struct block *previous = NULL, *tmp = NULL, *free_tmp = NULL;
	
    // Modify the allocated_list by deleting the node has been freed.
    // Traverse the allocated_list to set the pointer at the desired node.
    tmp = allocated_list;
    while (tmp != NULL){
        if((*tmp).addr != addr){
	    previous = tmp;
	    tmp = tmp->next;
	}
	else{
	    break;
	}
    }

    if(tmp == NULL){
	fprintf(stderr, "Invalid input: no such address in allocated_list!\n");
	return -1;
    }
				
    // Once we find the node, we can delete it.
    if (tmp == allocated_list){
	allocated_list = tmp->next;
    }
    else{
	previous->next = tmp->next;
    }
	
    // Traverse the freelist to insert the returned_free_node back at a proper place.
    free_tmp = freelist;
    while (free_tmp != NULL && (*free_tmp).addr < (*tmp).addr){
	previous = free_tmp;
	free_tmp = free_tmp->next;
    }
		
    // Once we find a node whose addr is larger than *returned_free_node.addr (or all the nodes in the list
    // have larger sizes), we can insert returned_free_node before this node (or at the beginning of the
    // list).
    if (free_tmp == freelist){
	tmp->next = freelist;
	freelist = tmp;
    }
    else{
	tmp->next = previous->next;
	previous->next = tmp;
    }
		
    return 0;
    }


/* Initialize the memory space used by smalloc,
 * freelist, and allocated_list
 * Note:  mmap is a system call that has a wide variety of uses.  In our
 * case we are using it to allocate a large region of memory. 
 * - mmap returns a pointer to the allocated memory
 * Arguments:
 * - NULL: a suggestion for where to place the memory. We will let the 
 *         system decide where to place the memory.
 * - PROT_READ | PROT_WRITE: we will use the memory for both reading
 *         and writing.
 * - MAP_PRIVATE | MAP_ANON: the memory is just for this process, and 
 *         is not associated with a file.
 * - -1: because this memory is not associated with a file, the file 
 *         descriptor argument is set to -1
 * - 0: only used if the address space is associated with a file.
 */
void mem_init(int size) {
    mem = mmap(NULL, size,  PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    if(mem == MAP_FAILED) {
         perror("mmap");
         exit(1);
    }
	
    // Initialize the freelist with size and address of mem
    freelist = (struct block*)malloc(sizeof(struct block));
    freelist->addr = mem;
    freelist->size = size;
    freelist->next = NULL;
    // Initialize an empty allocated_list
    allocated_list = NULL;
}

void mem_clean(){
    struct block *tmp = NULL;
    while(freelist != NULL){
	tmp = freelist->next;
	free(freelist);
	freelist = tmp;
    }
    freelist = NULL;

    while(allocated_list != NULL){
	tmp = allocated_list->next;
	free(allocated_list);
	allocated_list = tmp;
    }
    allocated_list = NULL;

}

