#include "FauxRacket.h"
#include <stdio.h>

int main(void)
{
	for( int i = 1; i <= 7; i++ )
	{
		printf( "Case %d: %d\n", i, interp_loop( parse( miread() ), NULL ).v.n );
	}
	return 0;
}
