

#pragma once

#include "CoreMinimal.h"
#include "B_Block.h"

/**
 * 
 */
class MC_FAKE_API B_Fluid : public B_Block
{
protected:
static UMaterial* FluidMaterial;

public:
    B_Fluid();

	bool IsSideOptimizable(EDirection Direction) override;
	UMaterialInstanceDynamic* GetMaterial(UObject* UObj) override;
};
