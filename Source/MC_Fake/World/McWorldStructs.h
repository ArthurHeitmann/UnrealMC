#pragma once

#include "CoreMinimal.h"
#include "ChunkFormCoords.h"
//#include "Blocks/Block.h"
//#include "ChunkCube.h"

struct ChunkCubeGenBufferElement {
	class ChunkCube* Cube;
	int NextGenStage;
};

struct ChunkGenBufferElement {
	int32 x;
	int32 y;
	class Chunk* Chunk;
};

struct BlockSetBufferElement {
	ChunkFormCoords3D CurrChunkPos;
	uint16 RelX, RelY, RelZ;
	class B_Block* BlockS;
	uint8 MinGenStage;

	BlockSetBufferElement() {}
	BlockSetBufferElement(int32 x, int32 y, int32 z, B_Block* b, uint8 MinGenerationStage)
	{
		int16 tmpRelX = x % 16;
		int16 tmpRelY = y % 16;
		RelZ = z % 16;
		if (tmpRelX < 0)
			tmpRelX += 16;
		if (tmpRelY < 0)
			tmpRelY += 16;
		RelX = tmpRelX;
		RelY = tmpRelY;
		CurrChunkPos.x = (int32) floorf(x / 16.f);
		CurrChunkPos.y = (int32) floorf(y / 16.f);
		CurrChunkPos.z = (int32) floorf(z / 16.f);

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
