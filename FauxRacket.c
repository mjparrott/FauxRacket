#include "FauxRacket.h"
#include "Sexp.h"
#include "AssociationList.h"
#include "dbg.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* parse: Take an s-expression representation of a program and convert it
          to an AST (abstract syntax tree)
 * prog: S-expression representation of the program.
 */
struct exp *parse( struct node *prog )
{
	assert(prog != NULL);
	
	if( prog->tag == LST )
	{
		//return parse
		debug( "Parsing a list" );
		struct exp *sublstParse = parse( prog->sublst );
		
		//(exp exp)
		if( ((sublstParse->type == FUN || sublstParse->type == APP) && prog->rest != NULL)
		|| (sublstParse->type == SYM && prog->sublst->rest != NULL) )
		{
			//NOTE: this code is repeated later on
			debug( "Parsing function application" );
			struct exp *sexp = malloc( sizeof( struct exp ) );
			check_mem(sexp);
			
			struct exp *func = sublstParse;
			struct exp *arg;
			if(sublstParse->type == SYM)
				arg = parse( prog->sublst->rest );
			else
				arg = parse( prog->rest );
			sexp->type = APP;
			sexp->e.funApp = (struct app){ .func = func, .arg = arg };
			return sexp;
		}
		else
		{
			return sublstParse;
		}
	}
	else if( prog->tag == NUM )
	{
		debug( "Parsing a number: %d", prog->num );
		struct exp *sexp = malloc( sizeof( struct exp ) );
		check_mem(sexp);
		
		sexp->type = NUMBER;
		sexp->e.n = prog->num;
		return sexp;
	}
	else if( prog->tag == NAME )
	{
		//(fun (id) exp)
		if( strcmp( prog->str, "fun" ) == 0 )
		{
			debug( "Parsing lambda function" );
			
			struct exp *sexp = malloc( sizeof( struct exp ) );
			check_mem(sexp);
			
			char* id = strdup(prog->rest->sublst->str); //TODO: error checking
			struct exp *body = parse( prog->rest->rest );
			struct fun func = (struct fun){ .id = id, .body = body };
			sexp->type = FUN;
			sexp->e.f = func;
			return sexp;
		}
		else if( strcmp( prog->str, "+" ) == 0 || strcmp( prog->str, "-" ) == 0
		|| strcmp( prog->str, "*" ) == 0 || strcmp( prog->str, "/" ) == 0 )
		{
			debug("Parsing binary structure.");
			
			struct exp *exp1 = parse( prog->rest );
			struct exp *exp2 = parse( prog->rest->rest );
			struct exp *fin = malloc( sizeof( struct exp ) );
			check_mem(fin);
			
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
			check_mem(fin);
			
			fin->type = IFZERO;
			fin->e.ifz = (struct ifzero){ test, texp, fexp };
			return fin;
		}
		//(sym exp), function application
		/*else if( prog->rest != NULL )
		{
			//NOTE: avoid repeating this code
			debug( "Parsing function application" );
			
			struct exp *sexp = malloc( sizeof( struct exp ) );
			check_mem(sexp);
			
			struct exp *func = malloc( sizeof( struct exp ) );
			check_mem(func);
			
			func->type = SYM;
			func->e.sym = prog->str;
			struct exp *arg = parse( prog->rest );
			sexp->type = APP;
			sexp->e.funApp = (struct app){ .func = func, .arg = arg };
			return sexp;
		}*/
		else
		{
			debug("Parsing symbol");
			
			struct exp *sym = malloc( sizeof( struct exp ) );
			check_mem(sym);
			
			sym->type = SYM;
			sym->e.sym = strdup(prog->str);
			return sym;
		}
	}
	else
	{
		sentinel("Non-existent tag found.");
	}

	sentinel("Returning nothing from parse.");
error:
	return NULL;
}

/* 
 * interp_loop: Take an AST and reduce it to a FauxRacket value (FRVal)
 * prog: the AST representation of the program
 * env: the environment containing symbol -> value mappings
 */
struct FRVal interp_loop( struct exp *prog, struct pair *env )
{
	//Initial continuation of interpreter is empty
	struct continuation *k = malloc( sizeof( struct continuation ) );
	check_mem(k);
	
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
				debug("Interpret bin structure");
				struct continuation *newk = malloc( sizeof( struct continuation ) );
				check_mem(newk);
			
				newk->type = K_BINL;
				newk->k.binL = (struct k_binL){ .op = convert_to_bin_type( prog->e.b.op ), 
							.cont = k, .rest = prog->e.b.right, .env = env };
				prog = prog->e.b.left;
				k = newk;
			}
			else if( prog->type == IFZERO )
			{
				struct continuation *newk = malloc( sizeof( struct continuation ) );
				check_mem(newk);
				
				newk->type = K_IFZERO;
				newk->k.ifzero = (struct k_ifzero){ .texp = prog->e.ifz.texp,
							.fexp = prog->e.ifz.fexp, .cont = k, .env = env };
				prog = prog->e.ifz.test;
				k = newk;
			}
			else if( prog->type == FUN )
			{
				debug( "Interpret fun structure" );
			   struct closure c = (struct closure){ .param = prog->e.f.id, .body = prog->e.f.body, .env = make_copy_env(env) };
			   val = (struct FRVal){ .type = FR_FUNCTION, .v.clos = c };
			   state = APPLY_CONT;
			}
			else if( prog->type == APP )
			{
				debug( "Interpret app structure" );
			   struct continuation *newk = malloc( sizeof( struct continuation ) );
			   check_mem(newk);
			   
			   newk->type = K_APPL;
			   newk->k.appL.arg = prog->e.funApp.arg;
			   newk->k.appL.env = make_copy_env(env);
			   newk->k.appL.cont = k;
			   
			   k = newk;
			   prog = prog->e.funApp.func;
			}
			else if( prog->type == NUMBER )
			{
				debug( "Interpret number: %d", prog->e.n );
				state = APPLY_CONT;
				val.v.n = prog->e.n;
			}
			else if( prog->type == SYM )
			{
				debug( "Interpret symbol" );
				struct pair *p = find( prog->e.sym, env );
				check(p, "Cannot find symbol %s", prog->e.sym);
				
				val = p->val;
				state = APPLY_CONT;
			}
		}
		else if( state == APPLY_CONT )
		{
			if( k->type == K_MT )
			{
				debug("Apply k_mt continuation");
				state = QUIT;
			}
			else if( k->type == K_BINL )
			{
				debug("Apply k_binl continuation");
				
				struct continuation *newk = malloc( sizeof( struct continuation ) );
				check_mem(newk);
				
				prog = k->k.binL.rest;
				newk->type = K_BINR;
				newk->k.binR = (struct k_binR){ .op = k->k.binL.op,
							.cont = k->k.binL.cont, .val = val.v.n };
				
				free(k);
				k = newk;
				state = INTERP;
				debug("Finished applying k_binl continuation");
			}
			else if( k->type == K_BINR )
			{
				debug("Apply k_binr continuation");
				
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
				debug( "Apply appL continuation" );
				
			   struct continuation *newk = malloc( sizeof( struct continuation ) );
			   check_mem(newk);
			   
			   prog = k->k.appL.arg;
			   newk->type = K_APPR;
			   newk->k.appR = (struct k_appR){ .clos = val.v.clos, .cont = k->k.appL.cont };
			   
			   env = k->k.appL.env;
			   free(k);
			   k = newk;
			   state = INTERP;
			}
			else if( k->type == K_APPR )
			{ 
				debug( "Apply appR continuation" );
				
			   struct continuation *temp = k;
			   env = push( k->k.appR.clos.param, val, k->k.appR.clos.env );
			   prog = k->k.appR.clos.body;
			   free(k);
			   k = temp->k.appR.cont;
			   state = INTERP;
			}
		}
	}
	free(k);
	//free_assoc_list( env );
	
	return val;

error:
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

/* free_ast: free an AST structure created by parse
 */
void free_ast( struct exp *ast )
{
	switch( ast->type )
	{
		case BIN:
			free_ast( ast->e.b.left );
			ast->e.b.left = NULL;
			free_ast( ast->e.b.right );
			ast->e.b.right = NULL;
			break;
		case IFZERO:
			free_ast( ast->e.ifz.test );
			ast->e.ifz.test = NULL;
			free_ast( ast->e.ifz.texp );
			ast->e.ifz.test = NULL;
			free_ast( ast->e.ifz.fexp );
			ast->e.ifz.fexp = NULL;
			break;
		case FUN:
			free_ast( ast->e.f.body );
			ast->e.f.body = NULL;
			free( ast->e.f.id );
			ast->e.f.id = NULL;
			break;
		case APP:
			free_ast( ast->e.funApp.func );
			ast->e.funApp.func = NULL;
			free_ast( ast->e.funApp.arg );
			ast->e.funApp.func = NULL;
			break;
		case SYM:
			free( ast->e.sym );
			ast->e.sym = NULL;
			break;
		case NUMBER:
			break;
		default:
			break;
	}
	free(ast);
	ast = NULL;
}
