
// handles trig function lookups

#include "mathtrigtable.h"


////////////////////////
/// MathTrigInitialize( )
////////////////////////
/// Wow, a real function! :)
/// Sets up our sin table with values. Should be called once at the start of the program, and that's all.
/// No shutdown is needed as these are not dynamic memory.
///
/// Input:
///		none
//
/// Returns:
///		none
////////////////////////
void MathTrigInitialize(void)
{
	int i = 0;
	for (; i < MATH_TRIG_TABLESIZE; i++)
	{
		pSin[i] = sinf(i * MATH_TRIG_INDEX_TO_RADIANS);
	}
}
