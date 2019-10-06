#pragma once

#include "CoreMinimal.h"
#include "Block.h"

class MC_FAKE_API B_Dirt : public Block
{
	
//protected:
	//static UMaterialInstanceDynamic* DirtMaterial;
public:
	B_Dirt();

	virtual Block* Clone() override;
};