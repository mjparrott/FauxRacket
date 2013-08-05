#include "FauxRacket.h"
#include <stdio.h>

int main(void)
{
	for( int i = 1; i <= 9; i++ )
	{
		printf( "%d\n", interp_loop( parse( miread() ), NULL ).v.n );
	}
	return 0;
}
