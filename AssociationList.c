/*
 * Association List to use for implementing environments
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "AssociationList.h"
#include "FauxRacket.h"

/* push: add a new pair onto the front of the association list
 */
struct pair *push( char* sym, struct FRVal frv, struct pair *lst )
{
	struct pair *newLst = malloc( sizeof( struct pair ) );
	if( newLst == NULL )
	{
		printf( "Error: out of memory\n" );
		abort();
	}
	
	newLst->sym = sym;
	newLst->val = frv;
	newLst->next = lst;
	
	return newLst;
}

/* pop: take off the first pair in the association list
 */
struct pair *pop( struct pair *lst )
{
	if( lst == NULL )
	{
	}
	
	struct pair *newLst = malloc( sizeof( struct pair ) );
	if( newLst == NULL )
	{
	}
	
	newLst = lst->next;
	free( lst );
	
	return newLst;
}

/* find: find the value with key sym in the association list lst
 */
struct pair *find( char* sym, struct pair *lst )
{
	DEBUG_PRINTF( "find %s\n", sym );
   struct pair *p = lst;
   while( p != NULL )
   {
      if( strcmp( sym, p->sym ) == 0 )
         return p;
      p = p->next;
   }
   DEBUG_PRINTF("found none\n");
   return NULL;
}

/* free_assoc_list: free all the memory allocated by lst
 */
void free_assoc_list( struct pair *lst )
{
	if( lst != NULL )
	{
		free( lst->sym );
		free_assoc_list( lst->next );
		free( lst );
	}
}
