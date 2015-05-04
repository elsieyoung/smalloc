/* Description of mytest.c: 
 * Basically, we want to test three specific scenarios in this test.
 * Step 1: 
 * First, we call smalloc four times with different size 10, 20, 30 and 40. 
 * Expected Result: 
 * There are 4 blocks in the allocated_list with size 10, 20, 30 and 40 
 * and the block in the freelist is updated correctly. 
 * 
 * Step 2:
 * Then we call sfree four times to free all the blocks we just allocated.
 * Expected Result: 
 * There is nothing in the allocated_list and 5 blocks in the freelist
 * including the 4 blocks we just freed from the allocated_list. 
 *
 * Step 3:
 * Then we call smalloc four more times with sizes exactly 20.
 * Expected Result: 
 * There are 4 blocks in the allocated_list with size 20 for each and there 
 * are 3 blocks left in the freelist in which 2 of them with size 20.
 * (Note: We first skip the first block with size 10 which is smaller than the
 * required size,
 * and allocate the second block with size 20,and split the third block into 
 * two halves: one with size 20 in the allocated_list,one with size 10 stayed 
 * in the freelist. At last we split the block with size 40 into two blocks 
 * with size 20 for each and move one to the allocated_list.)
 * In this step, we test all the possible scenarios below:
 * Scenario 1: Verify that smalloc can successfully allocate blocks of memory 
 * from the freelist when it finds a block that is larger than the required size
 * and can split this block into two blocks correctly. 
 * Scenario 2: Verify that smalloc can successfully allocate blocks of memory 
 * from the freelist when it finds a block of exactly the required size and
 * can take it from the freelist with the correct infomation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "smalloc.h"


#define SIZE 4096 * 64

/* My test for smalloc and sfree. */

int main(void) {

    mem_init(SIZE);

    printf("----Pre step----: \nCheck if the mem_init is executed successfully.\n\n");
    printf("List of allocated blocks:\n");
    print_allocated();
    printf("List of free blocks:\n");
    print_free();
    printf("Contents of allocated memory:\n");
    print_mem();
    
    char *ptrs[10];
    int i;

    /* Call smalloc 4 times */
    
    for(i = 0; i < 4; i++) {
        int num_bytes = (i+1)*10;
    
        ptrs[i] = smalloc(num_bytes);
        write_to_mem(num_bytes, ptrs[i], i);
	}

    printf("----First step----: \nWe allocate 4 blocks from freelist with size 10, 20, 30 and 40 respectively.\n\n");
    printf("List of allocated blocks:\n");
    print_allocated();
    printf("List of free blocks:\n");
    print_free();
    printf("Contents of allocated memory:\n");
    print_mem();

    /* Call sfree 4 times */
    printf("----Second step----:\nWe free all 4 blocks.\n\n");

    for(i = 0; i < 4; i++) {
    	printf("freeing %p result = %d\n", ptrs[i], sfree(ptrs[i]));
	}
	
    printf("List of allocated blocks:\n");
    print_allocated();
    printf("List of free blocks:\n");
    print_free();
    printf("Contents of allocated memory:\n");
    print_mem();

    /* Call smalloc 4 more times */
    
    for(i = 0; i < 4; i++) {
        int num_bytes = 20;
    
        ptrs[i] = smalloc(num_bytes);
        write_to_mem(num_bytes, ptrs[i], i);
	}
    
    printf("----Third step----:\nWe allocate 4 more blocks from freelist with size exactly 20.\n\n");
    printf("List of allocated blocks:\n");
    print_allocated();
    printf("List of free blocks:\n");
    print_free();
    printf("Contents of allocated memory:\n");
    print_mem();

    mem_clean();
    return 0;
}
