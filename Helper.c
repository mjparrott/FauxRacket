#include "Helper.h"
#include <stdio.h>
#include <ctype.h>

/* peekchar: Take a look at the next character in stdin without removing it
 */
int peekchar()
{
    return ungetc(getchar(), stdin);
}

/* skip_whitespace: Remove whitespace characters from the stdin until we
                    enounter a non-whitespace character
 */
void skip_whitespace()
{
	while ( isspace( peekchar() ) )
		getchar();
}
