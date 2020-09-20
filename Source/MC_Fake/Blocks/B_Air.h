#pragma once

#include "CoreMinimal.h"
#include "B_Block.h"
#include "BlockManager.h"

class MC_FAKE_API B_Air : public B_Block
{
	
public:
	B_Air();
	UMaterialInstanceDynamic* GetMaterial(UObject* UObj);

	TArray<uint8> GetBinaryData() override;
	virtual B_Block* Clone() override;
};
