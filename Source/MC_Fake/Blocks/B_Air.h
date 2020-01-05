#pragma once

#include "CoreMinimal.h"
#include "B_Block.h"

/**
 * 
 */
class MC_FAKE_API B_Air : public B_Block
{
	
public:
	static B_Air* AirRef;

	B_Air();
	UMaterialInstanceDynamic* GetMaterial(UObject* UObj);

	TArray<uint8> GetBinaryData() override;
	virtual B_Block* Clone() override;
};
