#include <math.h>
#include "MathUtils.h"


fPoint lerp(fPoint curr_value, fPoint trg_value, float factor)
{
	if (factor > 1.f)
	{
		factor = 1.f;
	}
	return 	curr_value * (1.f - factor) + trg_value * factor;
}
