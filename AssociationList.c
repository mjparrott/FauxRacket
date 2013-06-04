/*
 * Association List to use for implementing environments
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "AssociationList.h"

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

struct pair *find( char* sym, struct pair *lst )
{
	printf( "find\n" );
   struct pair *p = lst;
   while( p != NULL )
   {
      if( strcmp( sym, p->sym ) == 0 )
         return p;
      p = p->next;
   }
   return NULL;
}
