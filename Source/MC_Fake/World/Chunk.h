#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Blocks/Block.h"

struct ChunkGenMaps {
	TArray<TArray<int32>> HeightMap;
	TArray<TArray<float>> TemperatureMap;
	TArray<TArray<float>> PrecipitationMap;
	//River & etc. data
};

class MC_FAKE_API Chunk
{
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
	class AMcWorld* McWorld;
	USceneComponent* CubesRoot;
	class UBoxComponent* ChunkEnterTriggerBox;
	TMap<int8, class ChunkCube*> ChunkCubes;
	ChunkFormCoords2D Pos;
	ChunkGenMaps ChunkDataMaps;
	int LifeStage = 0;		//0: normal (visible), 1: Only shadows; no Mesh, 2: Hidden, 3: to be destroyed
	//bool bHasDataChanged = false;
	bool bHasFinishedGenerating = false;
	TQueue<int8> ChunkCubesCreatingBuffer;
	//double LastTimeUpdated = 0;
	/*Chunk* NorthChunk;
	Chunk* EastChunk;
	Chunk* SouthChunk;
	Chunk* WestChunk;*/


	TArray<BlockBreakingData> BreakingBlocks;

	void CreateChunkCube(int8 Pos);
	void DequeueChunkCubes();
	//bool ShouldFaceBePlacedBetween(Block* b1, Block* b2, EDirection Side);

protected:


public:
	USceneComponent* Root;

	Chunk(ChunkFormCoords2D Pos, class AMcWorld* McWorld);
	~Chunk();
	ChunkFormCoords2D GetPos();
	UFUNCTION()
	void ChunkEntered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//void SetData(const TArray<TArray<TArray<Block*>>>& NewData, bool bUpdateMesh);
	//void SetHasDataChanged(bool state = true);
	void SetHasFinishedGenerating(bool state);
	bool GetHasFinishedGenerating();
	//void UpdateMesh();
	void SetMeshLifeStage(int Stage);
	/* Loads new ChunkCubes if they now are in range */
	void UpdateChunkCubesLoading(int8 BaseHeight, int8 RangeDown, int8 RangeUp);
	Block* RegisterHitAt(const FHitResult& HitResult, class Item* Item);
	bool ContinueHit(class ABlockBreaking* Block, class Item* Item);
	void CancelBreaking(class Block* Block);
	TMap<int8, class ChunkCube*>& GetChunkCubes();
	Block* GetBlockAt(int x, int y, int z);
	Block*& GetBlockAtAsRef(int x, int y, int z);
	ChunkGenMaps& GetChunkGenMaps();
	/*void SetNorthChunk(Chunk* c);
	void SetEastChunk(Chunk* c);
	void SetSouthChunk(Chunk* c);
	void SetWestChunk(Chunk* c);*/
	void ToggleChunkBorders();

	//uint64 GetLastTimeUpdated();
	//void SetLastTimeUpdated(float NewTime);
	

	void Tick(float Delta);


};
