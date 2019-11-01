#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Blocks/Block.h"
#include "Chunk.generated.h"

struct ChunkGenMaps {
	TArray<TArray<int32>> HeightMap;
	TArray<TArray<float>> TemperatureMap;
	TArray<TArray<float>> PrecipitationMap;
	//River & etc. data
};

UCLASS()
class MC_FAKE_API AChunk : public AActor
{
	GENERATED_BODY()

	struct BlockBreakingData {
		class ABlockBreaking* Block;
		float HitTime;
		class Item* ItemUsed;
		int x; int y; int z;

		bool operator==(BlockBreakingData b)
		{
			return Block == b.Block && x == b.x && y == b.y && z == b.z;
		}
	};

private:
	UPROPERTY(EditAnywhere)
	USceneComponent* Root;
	class UBoxComponent* ChunkEnterTriggerBox;
	TMap<int8, class UChunkCube*> ChunkCubes;
	ChunkGenMaps ChunkDataMaps;
	class AMcWorld* McFWorld;
	int32 PosX, PosY;
	int LifeStage = 0;		//0: normal (visible), 1: Only shadows; no Mesh, 2: Hidden, 3: to be destroyed
	//bool bHasDataChanged = false;
	bool bHasFinishedGenerating = false;
	//double LastTimeUpdated = 0;
	/*AChunk* NorthChunk;
	AChunk* EastChunk;
	AChunk* SouthChunk;
	AChunk* WestChunk;*/


	TArray<BlockBreakingData> BreakingBlocks;

	void LoadChunkCube(int8 Pos);
	//bool ShouldFaceBePlacedBetween(Block* b1, Block* b2, EDirection Side);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type Reason) override;


public:	

	AChunk();
	UFUNCTION()
	void ChunkEntered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//void SetData(const TArray<TArray<TArray<Block*>>>& NewData, bool bUpdateMesh);
	void SetHasDataChanged(bool state = true);
	void SetHasFinishedGenerating(bool state);
	//void UpdateMesh();
	void SetMeshLifeStage(int Stage);
	/* Loads new ChunkCubes if they now are in range */
	void UpdateChunkCubesLoading(int8 BaseHeight, int8 RangeDown, int8 RangeUp);
	Block* RegisterHitAt(const FHitResult& HitResult, class Item* Item);
	bool ContinueHit(class ABlockBreaking* Block, class Item* Item);
	void CancelBreaking(class Block* Block);
	TMap<int8, class UChunkCube*>& GetChunkCubes();
	Block* GetBlockAt(int x, int y, int z);
	Block*& GetBlockAtAsRef(int x, int y, int z);
	ChunkGenMaps& GetChunkGenMaps();
	/*void SetNorthChunk(AChunk* c);
	void SetEastChunk(AChunk* c);
	void SetSouthChunk(AChunk* c);
	void SetWestChunk(AChunk* c);*/
	void ToggleChunkBorders();

	//uint64 GetLastTimeUpdated();
	//void SetLastTimeUpdated(float NewTime);
	

	virtual void Tick(float Delta) override;


};
