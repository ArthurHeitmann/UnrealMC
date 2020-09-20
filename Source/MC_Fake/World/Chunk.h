#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Blocks/B_Block.h"
#include "Chunk.generated.h"

struct ChunkGenMaps {
	TArray<TArray<float>> HeightMap;
	TArray<TArray<float>> TemperatureMap;
	TArray<TArray<float>> PrecipitationMap;
	//River & etc. data
};
UCLASS()
class MC_FAKE_API UChunk : public USceneComponent
{
GENERATED_BODY()
	
protected:
	bool bHasBeenDestroyed = false;
	UPROPERTY()
	class AMcWorld* McWorld = nullptr;
	UPROPERTY()
	TMap<int8, class UChunkCube*> ChunkCubes;
	FChunkFormCoords2D Pos;
	ChunkGenMaps ChunkDataMaps;
	bool bHasFinishedGenerating = false;
	bool bIsGenerating = false;

	void CreateChunkCube(int8 PosZ);

public:
	UChunk();
	virtual void BeginPlay() override;
	void Init(FChunkFormCoords2D pPos, class AMcWorld* pMcWorld);
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	FChunkFormCoords2D GetPos();
	void SetHasFinishedGenerating(bool bState);
	bool GetHasFinishedGenerating();
	bool GetIsGenerating();
	void SetIsGenerating(bool bState);
	class UChunkCube* GetChunkCube(int8 PosZ);
	/* Loads new ChunkCubes if they now are in range */
	void UpdateChunkCubesLoading(int8 BaseHeight, int8 RangeDown, int8 RangeUp);
	TMap<int8, class UChunkCube*>& GetChunkCubes();
	B_Block* GetBlockAt(int x, int y, int z);
	B_Block*& GetBlockAtAsRef(int x, int y, int z);
	ChunkGenMaps& GetChunkGenMaps();
	bool HasBeenDestroyed();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
