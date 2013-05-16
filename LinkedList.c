/*
 * Linked List to use for implementing environments
 */
#include <stdlib.h>
#include "LinkedList.h"

struct node *push( struct FRVal *frv, struct node *lst )
{
	struct node *newLst = malloc( sizeof( struct node ) );
	if( newLst == NULL )
	{
		printf( "Error: out of memory\n" );
		abort();
	}
	
	newLst->data = frv;
	newLst->next = lst;
	
	return node;
}

struct node *pop( struct node *lst )
{
	if( lst == NULL )
	{
	}
	
	struct node *newLst = malloc( sizeof( struct node ) );
	if( newLst == NULL )
	{
	}
	
	newLst = lst->next;
	free( lst->frv );
	free( lst );
	
	return newLst;
}
