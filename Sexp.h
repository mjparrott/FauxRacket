#ifndef SEXP_H
#define SEXP_H

struct node
{
	enum {NAME, NUM, LST} tag;
	int num;
	char *str;
	struct node *sublst;
	struct node *rest;
};

#define NSIZE sizeof(struct node)

struct token
{
	enum {LPAR, RPAR, STR, INT, TEOF} tag;
	int num;
	char *str;
};

struct token read_token();
int read_nat( int c );
char *read_id( int c );
struct node *miread();
struct node *token_to_node( struct token t );
struct node *read_list();
void free_sexp( struct node *sexp );

#endif
