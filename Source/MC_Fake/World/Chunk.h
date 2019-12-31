#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Blocks/Block.h"

struct ChunkGenMaps {
	TArray<TArray<float>> HeightMap;
	TArray<TArray<float>> TemperatureMap;
	TArray<TArray<float>> PrecipitationMap;
	//River & etc. data
};

class MC_FAKE_API Chunk
{
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
	bool bIsGenerating = false;
	TQueue<int8> ChunkCubesCreatingBuffer;
	//double LastTimeUpdated = 0;
	/*Chunk* NorthChunk;
	Chunk* EastChunk;
	Chunk* SouthChunk;
	Chunk* WestChunk;*/



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
	bool GetIsGenerating();
	void SetIsGenerating(bool val);
	//void UpdateMesh();
	void SetMeshLifeStage(int Stage);
	class ChunkCube* GetChunkCube(int8 PosZ);
	/* Loads new ChunkCubes if they now are in range */
	void UpdateChunkCubesLoading(int8 BaseHeight, int8 RangeDown, int8 RangeUp);
	B_Block* RegisterHitAt(const FHitResult& HitResult, class Item* Item);
	bool ContinueHit(class ABlockBreaking* Block, class Item* Item);
	void CancelBreaking(class B_Block* Block);
	TMap<int8, class ChunkCube*>& GetChunkCubes();
	B_Block* GetBlockAt(int x, int y, int z);
	B_Block*& GetBlockAtAsRef(int x, int y, int z);
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
