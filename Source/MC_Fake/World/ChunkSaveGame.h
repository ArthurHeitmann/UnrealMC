

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "../Blocks/B_Block.h"
#include "ChunkSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class MC_FAKE_API UChunkSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	int PosX, PosY;
	TArray<TArray<TArray<B_Block>>> ChunkBlockData;
	int NextGenerationStage;

	void ParseData(const TArray<TArray<TArray<B_Block*>>>& NewData);
	TArray<TArray<TArray<B_Block*>>> GetData();
};
