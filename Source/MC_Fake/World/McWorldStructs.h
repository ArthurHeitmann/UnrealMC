#pragma once

#include "CoreMinimal.h"
#include "ChunkFormCoords.h"
//#include "Blocks/Block.h"
//#include "ChunkCube.h"

struct ChunkCubeGenBufferElement {
	class ChunkCube* Cube;
	int NextGenStage;
	FRunnableThread* Thread = nullptr;
};

struct ChunkGenBufferElement {
	int32 x;
	int32 y;
	class Chunk* Chunk;
	FRunnableThread* Thread = nullptr;
};

struct BlockSetBufferElement {
	ChunkFormCoords3D CurrChunkPos;
	uint16 RelX, RelY, RelZ;
	class Block* BlockS;
	uint8 MinGenStage;

	BlockSetBufferElement() {}
	BlockSetBufferElement(int32 x, int32 y, int32 z, Block* b, uint8 MinGenerationStage)
	{
		RelX = x % 16;
		RelY = y % 16;
		RelZ = z % 16;
		if (RelX < 0)
			RelX += 16;
		if (RelY < 0)
			RelY += 16;
		CurrChunkPos.x = (int32) floorf(x / 16);
		CurrChunkPos.y = (int32) floorf(y / 16);
		CurrChunkPos.z = (int32) floorf(z / 16);

		BlockS = b;
		MinGenStage = MinGenerationStage;
	}
};

struct ChunkNeighbourUpdates {
	//North, East, South, West, Top, Bottom
	class ChunkCube* NewNChunk;
	class ChunkCube* NewEChunk;
	class ChunkCube* NewSChunk;
	class ChunkCube* NewWChunk;
	class ChunkCube* NewTChunk;
	class ChunkCube* NewBChunk;
};
