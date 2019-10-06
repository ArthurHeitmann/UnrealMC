

#pragma once

#include "CoreMinimal.h"
#include "Block.h"

/**
 * 
 */
class MC_FAKE_API B_Leaves : public Block
{
	
public:
	B_Leaves();

	bool IsBlockOpaque() override;
	virtual void OnBreak(class UWorld* World, FVector Location) override;

	virtual Block* Clone() override;
};
