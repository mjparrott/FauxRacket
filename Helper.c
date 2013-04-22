#include "Helper.h"
#include <stdio.h>
#include <ctype.h>

int peekchar()
{
    return ungetc(getchar(), stdin);
}

void skip_whitespace()
{
	while ( isspace( peekchar() ) )
		getchar();
}

bool has_next()
{
   skip_whitespace();
   return (peekchar() != EOF);
}
