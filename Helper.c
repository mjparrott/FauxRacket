#include "Helper.h"
#include <stdio.h>
#include <ctype.h>

extern FILE *input;

/* peekchar: Take a look at the next character in stdin without removing it
 */
int peekchar()
{
    return ungetc(getc(input), input);
}

/* skip_whitespace: Remove whitespace characters from the stdin until we
                    enounter a non-whitespace character
 */
void skip_whitespace()
{
	while ( isspace( peekchar() ) )
		getc(input);
}
