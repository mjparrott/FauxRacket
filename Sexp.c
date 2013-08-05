/* S-expression reading code courtesy of Prabhakar Ragde, Professor at the
   University of Waterloo.
   From C Learning Materials, from the course CS146
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Sexp.h"
#include "Helper.h"
#include "dbg.h"

/* read_token: Read an S-expression token
 */
struct token read_token()
{
	struct token t;
	skip_whitespace();
	int c = getchar();
	if( c == EOF )
	{
		t.tag = TEOF;
	}
	else if( c == '(' )
	{
		t.tag = LPAR;
	}
	else if( c == ')' )
	{
		t.tag = RPAR;
	}
	else if( isgraph(c) && !isdigit(c) )
	{
		t.tag = STR;
		t.str = read_id(c);
	}
	else if( isdigit(c) )
	{
		t.tag = INT;
		t.num = read_nat(c);
	}
	else
	{
		printf( "bad character: %c\n", c );
		abort();
	}
	return t;
}

/* read_nat: Read a natural number, starting with the digit c
 */
int read_nat( int c )
{
	int ans = c - '0';
	while( isdigit( peekchar() ) )
	{
		c = getchar();
		ans = ans * 10 + c - '0';
	}
	return ans;
}

/* read_id: Read a symbol, starting with the character c
 */
char *read_id( int c )
{
	char buf[100], *newstr;
	buf[0] = c;
	for( int i = 1; i < 100; i++ )
	{
		c = peekchar();
		if( isalpha(c) || isdigit(c) )
		{
			buf[i] = getchar();
		}
		else
		{
			buf[i] = '\0';
			newstr = malloc( strlen(buf) + 1 );
			check_mem(newstr);
			strcpy( newstr, buf );
			return newstr;
		}
	}

error:
	return NULL;
}

/* miread: Read an s-expression into a node structure
   Supported list elements:
   -List
   -Natural numbers
   -Symbols
 */
struct node *miread()
{
	struct token t;
	struct node *newn = NULL;
	t = read_token();
	
	if( t.tag == TEOF )
	{
		return NULL;
	}
	else if ( t.tag == LPAR )
	{
		newn = malloc(NSIZE);
		check_mem(newn);

		newn->tag = LST;
		newn->sublst = read_list();
		return newn;
	}
	else if( t.tag == RPAR )
	{
		printf( "unmatched right parenthesis\n" );
		abort();
	}
	else
	{
		token_to_node(t);
	}
	
error:
	return NULL;
}


/* token_to_node: Convert a token structure into a node structure
 */
struct node *token_to_node( struct token t )
{
	struct node *newn;
	newn = malloc(NSIZE);
	check_mem(newn);
	
	if( t.tag == INT )
	{
		newn->tag = NUM;
		newn->num = t.num;
	}
	else
	{
		newn->tag = NAME;
		newn->str = t.str;
	}
	
	return newn;

error:
	return NULL;
}

/* read_list: Read a list into a node structure
 */
struct node *read_list()
{
	struct token t;
	struct node *newn, *ans = NULL, **last = &ans;
	
	while(1)
	{
		t = read_token();
		if( t.tag == RPAR )
		{
			*last = NULL;
			return ans;
		}
		else if( t.tag == LPAR )
		{
			newn = malloc( NSIZE );
			if( newn == NULL )
			{
				printf( "Error: malloc out of memory\n" );
				abort();
			}
			
			newn->tag = LST;
			newn->sublst = read_list();
		}
		else if( t.tag == TEOF )
		{
			printf( "Unfinished list\n" );
			abort();
		}
		else
		{
			newn = token_to_node( t );
		}
		
		*last = newn;
		last = &newn->rest;
	}
	
	return newn;
}

/* free_sexp: free all the memory allocated by reading an s-expression
 */
void free_sexp( struct node *sexp )
{
	if( sexp != NULL )
	{
		switch( sexp->tag )
		{
			case NAME:
				free( sexp->str );
				break;
			case NUM:
				break;
			case LST:
				free_sexp( sexp->sublst );
				break;
			default:
				sentinel("Non-existent tag: %d", sexp->tag);
				break;
		}
		free_sexp( sexp->rest );
		free( sexp );
	}

error:
	return;
}
