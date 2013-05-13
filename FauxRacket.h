#ifndef FAUX_RACKET_H
#define FAUX_RACKET_H

#include "Sexp.h"

//Parsing structures
struct bin
{
	char op;
	struct exp *left;
	struct exp *right;
};
struct ifzero
{
	struct exp *test;
	struct exp *texp;
	struct exp *fexp;
};
struct fun
{
	char* id;
	struct exp *body;
};
struct app
{
	struct exp *func;
	struct exp *arg;
};
struct exp
{
	enum { BIN, IFZERO, FUN, APP, NUMBER } type;
	union
	{
		struct bin b;
		struct ifzero ifz;
		struct fun f;
		struct app funApp;
		int n;
	} e;
};


//Binary operations
enum Operation { ADDITION, MULTIPLICATION, SUBTRACTION, DIVISION };

//Continuation structures for inte
struct k_mt
{
};
struct k_binL
{
	enum Operation op;
	struct continuation *cont;
	struct exp *rest;
};
struct k_binR
{
	enum Operation op;
	struct continuation *cont;
	int val;
};
struct k_ifzero
{
	struct exp *texp;
	struct exp *fexp;
	struct continuation *cont;
};
struct continuation
{
	enum { K_MT, K_BINL, K_BINR, K_IFZERO } type;
	union
	{
		struct k_mt mt;
		struct k_binL binL;
		struct k_binR binR;
		struct k_ifzero ifzero;
	} k;
};

struct exp *parse( struct node *prog );
int convert_to_bin_type( char s );
int interp_loop( struct exp *prog );

#endif
