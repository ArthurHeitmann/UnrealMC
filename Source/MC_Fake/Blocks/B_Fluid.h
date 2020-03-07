

#pragma once

#include "CoreMinimal.h"
#include "B_Block.h"

/**
 * 
 */
class MC_FAKE_API B_Fluid : public B_Block
{
public:
    B_Fluid();

	bool IsSideOptimizable(EDirection Direction) override;
};
