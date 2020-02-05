

#pragma once

#include "CoreMinimal.h"
#include "B_Fluid.h"
#include "BlockManager.h"

/**
 * 
 */
class MC_FAKE_API B_Water : public B_Fluid
{
	


public:
	B_Water();
	UMaterialInstanceDynamic* GetMaterial(UObject* UObj) override;
	bool IsBlockOpaque() override;

	virtual B_Block* Clone() override;
};
