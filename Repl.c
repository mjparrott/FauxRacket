/* Implement a REPL for Faux Racket

 * For current syntax implemented, see Syntax.txt
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Sexp.h"
#include "FauxRacket.h"
#include "dbg.h"

FILE *input = NULL;

int main(int argc, char *argv[])
{
	struct node *prog;
	struct FRVal result;
	struct exp *parsed;
	
	check(argc <= 3, "Too many command line arguments passed - accepts file as \
		parameter as well as optional paramter to test against or nothing.");
	if(argc >= 2)
	{
		//First argument is the filename to read a Faux Racket program from
		input = fopen(argv[1], "r");
		check(input != NULL, "Could not open %s", argv[1]);
		
		//Interpret the Faux Racket program
		prog = miread();
		parsed = parse( prog );
		result = interp_loop( parsed, NULL );
		//TODO: free memory here
		
		if(argc == 3) //Second argument is output to test against
		{
			//Compare the interpreted result to the expected result in the file
			FILE *outputFile = fopen(argv[2], "r");
			check(outputFile != NULL, "Could not open %s", argv[2]);
			
			//NOTE: 50 characters may not be enough in the future
			char expectedOutput[50];
			fgets(expectedOutput, 50, outputFile);
			//This needs to be changed if there will output to test against with new lines in it
			if(expectedOutput[strlen(expectedOutput)-1] == '\n')
				expectedOutput[strlen(expectedOutput)-1] = '\0';
			
			//NOTE: Temporarily assume that result of interpreted program is an integer
			//Until other datatypes are added
			char interpretedOutput[50];
			sprintf(interpretedOutput, "%d", result.v.n);
			
			return strcmp(expectedOutput, interpretedOutput);
		}
		else //No output to test against, just print the result of the interpreted program
		{
			printf("%d\n", result.v.n);
			return 0;
		}
	}
	else
		input = stdin;
	
	printf( "Welcome to the Faux Racket interpreter!\n" );
	printf( "To use this, enter a program to be interpreted.\n" );
	printf( "To stop, enter the empty program ().\n\n" );
	
	while(1)
	{
		printf( "Enter a Faux Racket program to be interpreted:\n" );
		prog = miread();
		debug("Reading s-expression complete.\n");
		
		//Check for the representation of the empty program
		if( prog->tag == LST && prog->sublst == NULL )
			break;
			
		parsed = parse(prog);
		debug( "Parsing complete.\n" );
		result = interp_loop( parsed, NULL );
		printf( "%d\n", result.v.n );
		
		free_sexp( prog );
		free_ast( parsed );
		prog = NULL;
		parsed = NULL;
	}
	
	printf( "\nEnd\n" );
	return 0;

error:
	return 1;
}
