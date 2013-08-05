/* Implement a REPL for Faux Racket

 * For current syntax implemented, see Syntax.txt
 */
#include <stdio.h>
#include <stdlib.h>
#include "Sexp.h"
#include "FauxRacket.h"
#include "dbg.h"

int main(void)
{
	struct node *prog;
	struct FRVal result;
	struct exp *parsed;
	
	printf( "Welcome to the Faux Racket interpreter!\n" );
	printf( "To use this, enter a program to be interpreted.\n" );
	printf( "To stop, enter the empty program ().\n\n" );
	
	while(1)
	{
		printf( "Enter a Faux Racket program to be interpreted:\n" );
		prog = miread();
		debug("Reading s-expression complete.");
		
		//Check for the representation of the empty program
		if( prog->tag == LST && prog->sublst == NULL )
			break;
			
		parsed = parse(prog);
		debug( "Parsing complete." );
		result = interp_loop( parsed, NULL );
		printf( "%d\n", result.v.n );
		
		//free_sexp( prog );
		//free_ast( parsed );
		prog = NULL;
		parsed = NULL;
	}
	
	printf( "\nEnd\n" );
	return 0;
}
