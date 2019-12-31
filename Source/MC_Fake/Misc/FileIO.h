

#pragma once

#include "CoreMinimal.h"
#include "Blocks/B_Air.h"

struct ChunkData {
	uint8 GeneratorVersion;
	TArray<TArray<TArray<class B_Block*>>> BlockData;
	uint64 LastTimeUpdated;
	uint8 NextGenerationStage;

	ChunkData() {
		BlockData.SetNum(16);
		for (int x = 0; x < 16; x++)
		{
			BlockData[x].SetNum(16);
			for (int y = 0; y < 16; y++)
			{
				BlockData[x][y].Init(B_Air::AirRef, 256);
			}
		}
	}
};

/**
 * 
 */
class MC_FAKE_API FileIO
{

public:
	//TODO convert to int32

	static ChunkData LoadChunk(const FString& WorldName, int PosX, int PosY);
	static void SaveChunk(class Chunk*, const FString& Worldname);
	static TArray<uint8> ChunkToBytes(class Chunk* Chunk);
	static bool DoesChunkExist(const FString& WorldName, int PosX, int PosY);

};
