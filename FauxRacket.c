#include "FauxRacket.h"
#include "Sexp.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* parse: Take an s-expression representation of a program and convert it
          to an AST (abstract syntax tree)
 */
struct exp *parse( struct node *prog )
{
	if( prog == NULL )
	{
		printf( "Error parsing\n" );
		abort();
	}
	
	if( prog->tag == LST )
		return parse( prog->sublst );
	else if( prog->tag == NUM )
	{
		struct exp *sexp = malloc( sizeof( struct exp ) );
		if( sexp == NULL )
		{
			printf( "Error: out of memory\n" );
			abort();
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
				printf( "Error: out of memory\n" );
				abort();
			}
			
			fin->type = BIN;
			fin->e.b = (struct bin){ prog->str[0], exp1, exp2 };
			return fin;
		}
		else if( strcmp( prog->str, "ifzero" ) == 0 )
		{
			struct exp *test = parse( prog->rest );
			struct exp *texp = parse( prog->rest->rest );
			struct exp *fexp = parse( prog->rest->rest->rest );
			struct exp *fin = malloc( sizeof( struct exp ) );
			if( fin == NULL )
			{
				printf( "Error: out of memory\n" );
				abort();
			}
			
			fin->type = IFZERO;
			fin->e.ifz = (struct ifzero){ test, texp, fexp };
			return fin;
		}
	}
}

/* interp_loop: Take an AST and reduce it to a FauxRacket value
 */
int interp_loop( struct exp *prog )
{
	//Initial continuation of interpreter is empty
	struct continuation *k = malloc( sizeof( struct continuation ) );
	if( k == NULL )
	{
		printf( "Error: out of memory\n" );
		abort();
	}
	k->type = K_MT;
	//Possible interpreter states
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
				newk->k.binL = (struct k_binL){ .op = convert_to_bin_type( prog->e.b.op ), 
							.cont = k, .rest = prog->e.b.right };
				prog = prog->e.b.left;
				k = newk;
			}
			else if( prog->type == IFZERO )
			{
				struct continuation *newk = malloc( sizeof( struct continuation ) );
				if( newk == NULL )
				{
					printf( "Error: out of memory\n" );
					abort();
				}
				
				newk->type = K_IFZERO;
				newk->k.ifzero = (struct k_ifzero){ .texp = prog->e.ifz.texp,
							.fexp = prog->e.ifz.fexp, .cont = k };
				prog = prog->e.ifz.test;
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
					printf( "Error: out of memory\n" );
					abort();
				}
				newk->type = K_BINR;
				newk->k.binR = (struct k_binR){ .op = k->k.binL.op,
							.cont = k->k.binL.cont, .val = val };
				
				prog = k->k.binL.rest;
				free(k);
				k = newk;
				state = INTERP;
			}
			else if( k->type == K_BINR )
			{
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
			else if( k->type == K_IFZERO )
			{
				struct contination *temp = k;
				
				if( val == 0 )
				{
					prog = k->k.ifzero.texp;
				}
				else
				{
					prog = k->k.ifzero.fexp;
				}
				
				k = k->k.ifzero.cont;
				free(temp);
				state = INTERP;
			}
		}
	}
	free(k);
	
	return val;
}

/* convert_to_bin_type: convert a character binary operator to the correct
                        enumeration type
 */
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
