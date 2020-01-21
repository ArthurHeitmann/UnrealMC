#pragma once

#include "CoreMinimal.h"
#include "B_Block.h"
#include "BlockManager.h"

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

BlockStaticRegistryImplementation(B_Air, new B_Air())

//class B_AirHelper
//{
//	B_Air Initializer;
//public: B_AirHelper()
//{
//	BlockManager::RegisterStaticBlock(FName("B_Air"), new B_Air());
//}
//};
//B_AirHelper B_AirInit;