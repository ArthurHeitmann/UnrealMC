#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Blocks/B_Block.h"

struct ChunkGenMaps {
	TArray<TArray<float>> HeightMap;
	TArray<TArray<float>> TemperatureMap;
	TArray<TArray<float>> PrecipitationMap;
	//River & etc. data
};

class MC_FAKE_API Chunk
{
protected:
	class AMcWorld* McWorld;
	USceneComponent* CubesRoot;
	TMap<int8, class ChunkCube*> ChunkCubes;
	ChunkFormCoords2D Pos;
	ChunkGenMaps ChunkDataMaps;
	bool bHasFinishedGenerating = false;
	bool bIsGenerating = false;

	void CreateChunkCube(int8 PosZ);

public:
	USceneComponent* Root;

	Chunk(ChunkFormCoords2D Pos, class AMcWorld* McWorld);
	~Chunk();
	ChunkFormCoords2D GetPos();
	void SetHasFinishedGenerating(bool bState);
	bool GetHasFinishedGenerating();
	bool GetIsGenerating();
	void SetIsGenerating(bool bState);
	class ChunkCube* GetChunkCube(int8 PosZ);
	/* Loads new ChunkCubes if they now are in range */
	void UpdateChunkCubesLoading(int8 BaseHeight, int8 RangeDown, int8 RangeUp);
	TMap<int8, class ChunkCube*>& GetChunkCubes();
	B_Block* GetBlockAt(int x, int y, int z);
	B_Block*& GetBlockAtAsRef(int x, int y, int z);
	ChunkGenMaps& GetChunkGenMaps();

	void Tick(float Delta);


};
