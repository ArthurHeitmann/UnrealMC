

#pragma once

#include "CoreMinimal.h"
#include "B_Block.h"

/**
 * 
 */
class MC_FAKE_API B_Water : public B_Block
{
	


public:
	B_Water();
	UMaterialInstanceDynamic* GetMaterial(UObject* UObj) override;
	bool IsBlockOpaque() override;

	virtual B_Block* Clone() override;
};
