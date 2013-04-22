#include "FauxRacket.h"
#include "Sexp.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct exp *parse( struct node *prog )
{
	if( prog->tag == LST )
		return parse( prog->sublst );
	else if( prog->tag == NUM )
	{
		struct exp *sexp = malloc( sizeof( struct exp ) );
		if( sexp == NULL )
		{
		}
		sexp->type = NUMBER;
		sexp->e.n = prog->num;
		return sexp;
	}
	else if( prog->tag == NAME )
	{
		if( strcmp( prog->str, "+" ) == 0 || strcmp( prog->str, "-" ) == 0
		|| strcmp( prog->str, "*" ) == 0 || strcmp( prog->str, "/" ) == 0 )
		{
			struct exp *exp1 = parse( prog->rest );
			struct exp *exp2 = parse( prog->rest->rest );
			struct exp *fin = malloc( sizeof( struct exp ) );
			if( fin == NULL )
			{
			}
			
			fin->type = BIN;
			fin->e.b = (struct bin){ prog->str[0], exp1, exp2 };
			return fin;
		}
	}
}

int interp_loop( struct exp *prog )
{
	struct continuation *k = malloc( sizeof( struct continuation ) );
	k->type = K_MT;
	enum { INTERP, APPLY_CONT, QUIT } state = INTERP;
	int val = 0; //Meaningless initiailization value, shouldn't be relevant
	
	while( state != QUIT )
	{
		if( state == INTERP )
		{
			if( prog->type == BIN )
			{
				struct continuation *newk = malloc( sizeof( struct continuation ) );
				if( newk == NULL )
				{
					printf( "Error: out of memory\n" );
					abort();
				}
			
				newk->type = K_BINL;
				newk->k.binL = (struct k_binL){ convert_to_bin_type( prog->e.b.op ), k, prog->e.b.right };
				prog = prog->e.b.left;
				k = newk;
			}
			else if( prog->type == NUMBER )
			{
				state = APPLY_CONT;
				val = prog->e.n;
			}
		}
		else if( state == APPLY_CONT )
		{
			if( k->type == K_MT )
			{
				state = QUIT;
			}
			else if( k->type == K_BINL )
			{
				struct continuation *newk = malloc( sizeof( struct continuation ) );
				if( newk == NULL )
				{
					printf( "Error: out of memory" );
					abort();
				}
				newk->type = K_BINR;
				newk->k.binR = (struct k_binR){ k->k.binL.op, k->k.binL.cont, val };
				
				prog = k->k.binL.rest;
				k = newk;
				state = INTERP;
			}
			else if( k->type == K_BINR )
			{
				//TODO: free memory (?)
				struct continuation *temp = k;
				
				if( k->k.binR.op == ADDITION )
				{
					val = k->k.binR.val + val;
				}
				else if( k->k.binR.op == MULTIPLICATION )
				{
					val = k->k.binR.val * val;
				}
				else if( k->k.binR.op == SUBTRACTION )
				{
					val = k->k.binR.val - val;
				}
				else if( k->k.binR.op == DIVISION )
				{
					val = k->k.binR.val / val;
				}
				
				k = k->k.binR.cont;
				free(temp);
			}
		}
	}
	free(k);
	
	return val;
}

//Think about making this return an enum value to make it clearer
int convert_to_bin_type( char s )
{
	if( s == '+' )
		return ADDITION;
	else if( s == '*' )
		return MULTIPLICATION;
	else if( s == '-' )
		return SUBTRACTION;
	else if( s == '/' )
		return DIVISION;
		
	return -1;
}	
