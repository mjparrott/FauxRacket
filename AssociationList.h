#ifndef ASSOCIATION_LIST_H
#define ASSOCIATION_LIST_H

#include "FauxRacket.h"

struct pair
{
   char* sym;
	struct FRVal val;
	struct pair *next;
};

struct pair *push( char* sym, struct FRVal frv, struct pair *lst );
struct pair *pop( struct pair *lst );
struct pair *find( char* sym, struct pair *lst );
struct pair *make_copy_env(struct pair *env);
void free_assoc_list( struct pair *lst );

#endif
