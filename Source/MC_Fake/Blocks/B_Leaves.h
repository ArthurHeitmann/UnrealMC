

#pragma once

#include "CoreMinimal.h"
#include "B_Block.h"

/**
 * 
 */
class MC_FAKE_API B_Leaves : public B_Block
{
protected:
	static UMaterial* WavyMaterialBase; 
	
public:
	B_Leaves();

	bool IsBlockOpaque() override;
	bool IsSideOptimizable(EDirection Direction) override;
	virtual void OnBreak(class UWorld* World, FVector Location) override;

	UMaterialInstanceDynamic* GetMaterial(UObject* UObj) override;
	
	virtual B_Block* Clone() override;
};
