/*
 * Association List to use for implementing environments
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "AssociationList.h"
#include "FauxRacket.h"
#include "dbg.h"

/* push: add a new pair onto the front of the association list
 */
struct pair *push( char* sym, struct FRVal frv, struct pair *lst )
{
	struct pair *newLst = malloc( sizeof( struct pair ) );
	check_mem(newLst);
	
	newLst->sym = sym;
	newLst->val = frv;
	newLst->next = lst;
	
	return newLst;

error:
	return NULL;
}

/* pop: take off the first pair in the association list
 */
struct pair *pop( struct pair *lst )
{
	if( lst == NULL )
	{
		log_err( "Cannot pop an empty list." );
		return NULL;
	}
	
	struct pair *newLst = malloc( sizeof( struct pair ) );
	if( newLst == NULL )
	{
		printf( "Error: out of memory\n" );
		abort();
	}
	
	newLst = lst->next;
	free( lst );
	
	return newLst;
}

/* find: find the value with key sym in the association list lst
 */
struct pair *find( char* sym, struct pair *lst )
{
	debug( "Looking for %s in the list", sym );
	
   struct pair *p = lst;
   while( p != NULL )
   {
      if( strcmp( sym, p->sym ) == 0 )
         return p;
      p = p->next;
   }
   debug("Did not find the symbol.");
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
