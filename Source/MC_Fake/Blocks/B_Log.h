

#pragma once

#include "CoreMinimal.h"
#include "B_Block.h"

/**
 * 
 */
class MC_FAKE_API B_Log : public B_Block
{
public:
    B_Log();

	TArray<FVector2D> GetTopUVs() override;
	TArray<FVector2D> GetFrontUVs() override;
	TArray<FVector2D> GetRightUVs() override;
	TArray<FVector2D> GetBottomUVs() override;
	TArray<FVector2D> GetLeftUVs() override;
	TArray<FVector2D> GetBackUVs() override;

	TArray<FVector2D> GetAllUVs() override;
};
