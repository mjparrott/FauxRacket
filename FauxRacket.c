#include "FauxRacket.h"
#include "Sexp.h"
#include "AssociationList.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* parse: Take an s-expression representation of a program and convert it
          to an AST (abstract syntax tree)
 * prog: 
 */
struct exp *parse( struct node *prog )
{
	if( prog == NULL )
	{
		printf( "Error parsing\n" );
		abort();
	}
	
	if( prog->tag == LST )
	{
		//(exp exp)
		if( prog->rest != NULL )
		{
			struct exp *sexp = malloc( sizeof( struct exp ) );
			if( sexp == NULL )
			{
				printf( "Error: out of memory\n" );
				abort();
			}
			
			struct exp *func = parse( prog->sublst );
			struct exp *arg = parse( prog->rest );
			sexp->type = APP;
			sexp->e.funApp = (struct app){ .func = func, .arg = arg };
			return sexp;
		}
		else
			return parse( prog->sublst );
	}
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
		//(fun (id) exp)
		if( strcmp( prog->str, "fun" ) == 0 )
		{
			struct exp *sexp = malloc( sizeof( struct exp ) );
			if( sexp == NULL )
			{
				printf( "Error: out of memory\n" );
				abort();
			}
			
			char* id = prog->rest->sublst->str; //TODO: error checking
			struct exp *body = parse( prog->rest->rest );
			struct fun func = (struct fun){ .id = id, .body = body };
			sexp->type = FUN;
			sexp->e.f = func;
			return sexp;
		}
		else if( strcmp( prog->str, "+" ) == 0 || strcmp( prog->str, "-" ) == 0
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
		else
		{
			struct exp *sym = malloc( sizeof( struct exp ) );
			if( sym == NULL )
			{
				printf( "Error: out of memory\n" );
				abort();
			}
			
			sym->type = SYM;
			sym->e.sym = prog->str;
			return sym;
		}
	}
}

/* 
 * interp_loop: Take an AST and reduce it to a FauxRacket value
 * prog: the AST representation of the program
 * env: the environment containing symbol -> value mappings
 */
struct FRVal interp_loop( struct exp *prog, struct pair *env )
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
	struct FRVal val = { .type = FR_NUMBER, .v.n = 0 }; //Meaningless initiailization value, shouldn't be relevant
	
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
			else if( prog->type == FUN )
			{
			   struct closure c = (struct closure){ .param = prog->e.f.id, .body = prog->e.f.body, .env = env };
			   val = (struct FRVal){ .type = FR_FUNCTION, .v.clos = c };
			   state = APPLY_CONT;
			}
			else if( prog->type == APP )
			{
			   struct continuation *newk = malloc( sizeof( struct continuation ) );
			   if( newk == NULL )
			   {
			      printf( "Error: out of memory\n" );
			      abort();
			   }
			   
			   newk->type = K_APPL;
			   newk->k.appL.arg = prog->e.funApp.arg;
			   newk->k.appL.env = env;
			   newk->k.appL.cont = k;
			   
			   k = newk;
			   prog = prog->e.funApp.func;
			}
			else if( prog->type == NUMBER )
			{
				state = APPLY_CONT;
				val.v.n = prog->e.n;
			}
			else if( prog->type == SYM )
			{
				struct pair *p = find( prog->e.sym, env );
				if( p == NULL )
				{
					printf( "Error: cannot find symbol %s\n", prog->e.sym );
					abort();
				}
				
				val = p->val;
				state = APPLY_CONT;
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
				prog = k->k.binL.rest;
				
				newk->type = K_BINR;
				newk->k.binR = (struct k_binR){ .op = k->k.binL.op,
							.cont = k->k.binL.cont, .val = val.v.n };
				
				free(k);
				k = newk;
				state = INTERP;
			}
			else if( k->type == K_BINR )
			{
				struct continuation *temp = k;
				
				if( k->k.binR.op == ADDITION )
				{
					val.v.n = k->k.binR.val + val.v.n;
				}
				else if( k->k.binR.op == MULTIPLICATION )
				{
					val.v.n = k->k.binR.val * val.v.n;
				}
				else if( k->k.binR.op == SUBTRACTION )
				{
					val.v.n = k->k.binR.val - val.v.n;
				}
				else if( k->k.binR.op == DIVISION )
				{
					val.v.n = k->k.binR.val / val.v.n;
				}
				
				k = k->k.binR.cont;
				free(temp);
			}
			else if( k->type == K_IFZERO )
			{
				struct continuation *temp = k;
				
				if( val.v.n == 0 )
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
			else if( k->type == K_APPL )
			{
			   struct continuation *newk = malloc( sizeof( struct continuation ) );
			   if( newk == NULL )
			   {
			      printf( "Error: out of memory\n" );
			      abort();
			   }
			   prog = k->k.appL.arg;
			   
			   newk->type = K_APPR;
			   newk->k.appR = (struct k_appR){ .clos = val.v.clos, .cont = k->k.appL.cont };
			   
			   free(k);
			   k = newk;
			   state = INTERP;
			}
			else if( k->type == K_APPR )
			{
			   struct continuation *temp = k;
			   env = push( k->k.appR.clos.param, val, env );
			   prog = k->k.appR.clos.body;
			   free(k);
			   k = temp->k.appR.cont;
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
