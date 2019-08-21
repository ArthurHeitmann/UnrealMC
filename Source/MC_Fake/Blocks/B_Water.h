

#pragma once

#include "CoreMinimal.h"
#include "Block.h"

/**
 * 
 */
class MC_FAKE_API B_Water : public Block
{


public:
	B_Water();
	UMaterialInstanceDynamic* GetMaterial(UObject* UObj) override;
	bool IsBlockOpaque() override;

	virtual Block* Clone() override;
};
