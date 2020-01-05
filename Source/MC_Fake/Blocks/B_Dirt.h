#pragma once

#include "CoreMinimal.h"
#include "B_Block.h"

class MC_FAKE_API B_Dirt : public B_Block
{
	
public:
	B_Dirt();

	virtual B_Block* Clone() override;
};