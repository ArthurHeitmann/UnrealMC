

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "../Blocks/Block.h"
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
	TArray<TArray<TArray<Block>>> ChunkBlockData;
	int NextGenerationStage;

	void ParseData(const TArray<TArray<TArray<Block*>>>& NewData);
	TArray<TArray<TArray<Block*>>> GetData();
};
