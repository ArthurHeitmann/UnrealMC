


#include "B_Fluid.h"

B_Fluid::B_Fluid()
{
	BlockModelType = FLUID;
}

bool B_Fluid::IsSideOptimizable(EDirection Direction)
{
	return false;
}
