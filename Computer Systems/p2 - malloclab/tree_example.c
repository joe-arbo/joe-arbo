#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include "tree.h"  
#include <assert.h>

struct boundary_tag {
    int inuse:1;        // inuse bit
    int size:31;        // size of block, in words
};

struct block {
    struct boundary_tag header; 	/* offset 0, at address 4 mod 8 */
    union { 				/* offset 4, at address 0 mod 8 */
        char payload[0];
        RB_ENTRY(block) node;
    };

};

/*Compares elements a to b. Returns -1 if smaller and 1 if bigger
  0 is never returned as ties are broken by their address. Better implementations might
  put duplicate sized blocks into a linked list */
static int compare_size(struct block * a, struct block * b)
{
    if (a->header.size < b->header.size)
        return -1;
    else if (a->header.size > b->header.size)
	return 1;
    else
	return a < b ? -1 : 1;    
}

//Declare an array of 5 rb tree named mytree whose head is referenced by tree
RB_HEAD(mytree, block) tree[5];

/* If you only use 1 tree, use the following */
//RB_HEAD(mytree, block) tree;

/*Generate a rb tree named mytree that contains struct blocks which are 
  linked by RB_ENTRY's called node and uses the compare_size function
  to compare the elements */
RB_GENERATE_STATIC(mytree, block, node, compare_size); 

int main(int argc, char *argv[])  
{  
    RB_INIT(&tree[1]);
    struct block a;
    a.header.size = 5;
 
    RB_INSERT(mytree, &tree[1], &a);
    struct block c;
    c.header.size = 4;
    struct block * p = RB_NFIND(mytree, &tree[1], &c);
    if (p != NULL)
        RB_REMOVE(mytree, &tree[1], p);
    assert(RB_EMPTY(&tree[1]));
   return EXIT_SUCCESS;  
}  
