

#pragma once

#include "CoreMinimal.h"
#include "B_Block.h"

/**
 * 
 */
class MC_FAKE_API B_Leaves : public B_Block
{
	
public:
	B_Leaves();

	bool IsBlockOpaque() override;
	virtual void OnBreak(class UWorld* World, FVector Location) override;

	virtual B_Block* Clone() override;
};
