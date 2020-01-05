#pragma once

#include "CoreMinimal.h"
#include "ChunkFormCoords.h"

struct ChunkCubeGenBufferElement {
	class ChunkCube* Cube;
	int NextGenStage;
};

struct ChunkGenBufferElement {
	int32 X;
	int32 Y;
	class Chunk* Chunk;
};

struct BlockSetBufferElement {
	ChunkFormCoords3D CurrChunkPos;
	uint16 RelX, RelY, RelZ;
	class B_Block* BlockS;
	uint8 MinGenStage;

	BlockSetBufferElement(): RelX(0), RelY(0), RelZ(0), BlockS(nullptr), MinGenStage(0) {}

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
		CurrChunkPos.X = static_cast<int32>(floorf(x / 16.f));
		CurrChunkPos.Y = static_cast<int32>(floorf(y / 16.f));
		CurrChunkPos.Z = static_cast<int32>(floorf(z / 16.f));

		BlockS = b;
		MinGenStage = MinGenerationStage;
	}
};

struct ChunkNeighborUpdates {
	//North, East, South, West, Top, Bottom
	class ChunkCube* NewNChunk;
	class ChunkCube* NewEChunk;
	class ChunkCube* NewSChunk;
	class ChunkCube* NewWChunk;
	class ChunkCube* NewTChunk;
	class ChunkCube* NewBChunk;
};
