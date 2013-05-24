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

//Interpreter structures
struct closure
{
   char* param;
   struct exp *body;
   struct pair *env;
};

//Binary operations
enum Operation { ADDITION, MULTIPLICATION, SUBTRACTION, DIVISION };

//Continuation structures for interpreter
struct k_mt
{
};
struct k_binL
{
	enum Operation op;
	struct continuation *cont;
	struct exp *rest;
	struct pair *env;
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
struct k_appL
{
   struct exp *arg;
   struct pair *env;
   struct continuation *cont;
};
struct k_appR
{
   struct closure *clos;
   struct continuation *cont;
};
struct continuation
{
	enum { K_MT, K_BINL, K_BINR, K_IFZERO, K_APPL, K_APPR } type;
	union
	{
		struct k_mt mt;
		struct k_binL binL;
		struct k_binR binR;
		struct k_ifzero ifzero;
		struct k_appL appL;
		struct k_appR appR;
	} k;
};

//Definition of a FauxRacket value
struct FRVal
{
	enum { FR_NUMBER, FR_FUNCTION } type;
	union
	{
		int n;
		struct closure clos; //TODO: should this be a closure?
	} v;
};

struct exp *parse( struct node *prog );
int convert_to_bin_type( char s );
struct FRVal interp_loop( struct exp *prog, struct pair *env );

#endif
