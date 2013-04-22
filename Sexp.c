#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Sexp.h"
#include "Helper.h"


//Functions
struct token read_token()
{
	struct token t;
	skip_whitespace();
	int c = getchar();
	if( c == EOF)
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
			strcpy( newstr, buf );
			return newstr;
		}
	}
}

struct node *miread()
{
	struct token t;
	struct node *newn;
	t = read_token();
	
	if( t.tag == TEOF )
	{
		return NULL;
	}
	else if ( t.tag == LPAR )
	{
		newn = malloc(NSIZE);
		if( newn == NULL )
		{
			printf( "Error: malloc out of memory\n" );
			abort();
		}
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
}

struct node *token_to_node( struct token t )
{
	struct node *newn;
	newn = malloc(NSIZE);
	if( newn == NULL )
	{
		printf( "Error: malloc out of memory\n" );
		abort();
	}
	
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
}

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
