/*** NO CHANGES TO THIS FILE ARE ALLOWED EXCEPT WHERE INDICATED ***/
/* MAB management functions for HOSTD */

/* Include files */
#include "mab.h"


/*** CODE COMMENTS MAY BE ADDED BEYOND THIS POINT ***/

/*** START OF SECTION MARKER ***/
/*** ADDITIONAL FUNCTION PROTOTYPES MAY BE ADDED HERE ***/
MabPtr createnullMab(int ,int);
/*** END OF SECTION MARKER ***/

/*******************************************************
 * MabPtr memAlloc (MabPtr * lists, int size);
 *    - allocate a memory block
 *
 * returns address of block or NULL if failure
 *******************************************************/
MabPtr memAlloc(MabPtr * lists, int size)
{
	MabPtr m;//initialising  to be assigned to the head of the lsit
	MabPtr Block = NULL;//return being assigned to null
	//printf("size is %d\n",size);
		m = *lists;
		while(m != NULL)
		{
			if(m->allocated == FALSE && m->size >= size)
			{
				m->allocated = TRUE;//allocating the block we are goign to split
				Block = memSplit(lists,m,size);//spliting the block
				break;
			}
			m = m->next;//iterating the list
		}
		//printf("size of the block is %d\n",Block->size);
    return Block;//returng the block
}

//MabPtr

/*******************************************************
 * MabPtr memFree (MabPtr * lists, MabPtr mab);
 *    - de-allocate a memory block
 *
 * returns address of block or merged block
 *******************************************************/
MabPtr memFree(MabPtr * lists, MabPtr m)
{
	
	m->allocated=FALSE;
	if(m->parent!=NULL)
	{
		//puts("merging");
		 m=memMerge(lists,m->parent);//reccursivelly calling memerge on the list
	}
	else
	{
		return m;//returning m
	}
}

/*******************************************************
 * MabPtr memMerge(MabPtr * lists, Mabptr m);
 *    - merge m recursively with its Buddy if free
 *
 * returns a pointer to one of the following:
 * - merged block if m's Buddy is free
 * - m if m is free but its Buddy is not
 * - NULL if there is an error
 *******************************************************/
MabPtr memMerge(MabPtr * lists, MabPtr m)
{
	MabPtr left;//creating 2 variables
	MabPtr right;

	if(m->left_child!=NULL)
	{
		left=m->left_child;//assiging it to the left and right child
		right=m->right_child;
	}
	else
	{
		return m;
	}
	if(left->allocated==FALSE && right->allocated==TRUE)
	{
		return m->left_child;//assigning to m's left
	}
	if(left->allocated==FALSE && right->allocated==FALSE)
	{
		m->left_child=NULL;//performign merge
		m->right_child=NULL;
		if(m->parent!=NULL)
		{
			return memMerge(lists,m->parent);//return the merged block
		}
	}
	
}

/*******************************************************
 * MabPtr memSplit(MabPtr * lists, MabPtr m, int size);
 *    - split m into two if needed
 *
 * returns a pointer to m or one of its children
 * returns NULL if there is an error
 *******************************************************/
MabPtr memSplit(MabPtr * lists, MabPtr m, int size)
{
	if(size==m->size)
	{
		return m;
	}
	if(size > m->size/2)
	{
		return m;
	}
	else
	{
			
		MabPtr leftChild  = createnullMab(m->offset,m->size/2);//creatign a leftChild
		MabPtr rightChild = createnullMab((m->offset+m->size/2),m->size/2); //creatign a left child assigning the offset as well
		
		leftChild->prev=m->prev;
		rightChild->next=m->next;
		rightChild->prev=leftChild;
		leftChild->parent=m;
		rightChild->parent=m;
		
		m->left_child = leftChild;
		m->right_child = rightChild;

		leftChild->next = rightChild;
		//checkign to see if m si the head of the list
		if(m->prev != NULL)
		{
			
			m->prev->next = leftChild;//adding to the list 
		}
		else
		{
			*lists = leftChild;//adding to the list 
		}

		if(m->next != NULL)
		{
			m->next->prev = rightChild;
		}
		m->next = NULL;
		m->prev = NULL;

		if(leftChild->size==size)
		{
			leftChild->allocated=TRUE;
			return leftChild;//returning the memory that can be allocated
		}
		if(leftChild->size > size)
		{
			leftChild->allocated=TRUE;
			return memSplit(lists,leftChild,size);//reccursivelly calling memsplit
		}
	
	}

	return NULL;
    
}
//creating a null MAB

MabPtr createnullMab(int offset,int size){
	MabPtr mem = (MabPtr)malloc(sizeof(Mab));
	mem->size = size;
	mem->offset = offset;
	mem->allocated = FALSE;
	mem->next = NULL;
	mem->prev = NULL;
	mem->parent=NULL;
	mem->left_child=NULL;
	mem->right_child=NULL;
	return mem;
}

/*** START OF SECTION MARKER ***/
/*** ADDITIONAL FUNCTION IMPLEMENTATIONS MAY BE ADDED HERE ***/
/*** END OF SECTION MARKER ***/
/*** END OF CODE; DO NOT ADD MATERIAL BEYOND THIS POINT ***/
