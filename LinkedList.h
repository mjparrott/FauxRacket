#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "FauxRacket.h"

struct node
{
	struct FRVal *data;
	struct node *next;
};

struct node *push( struct FRVal *frv, struct node *lst );
struct node *pop( struct node *lst );

#endif
