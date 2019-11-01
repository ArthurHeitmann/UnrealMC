#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChunkGenerator.h"
#include "Containers/Queue.h"
#include "Enums.h"
#include "McWorld.generated.h"

struct ChunkFormCoords3D {
	int32 x, y, z;

	bool operator==(const ChunkFormCoords3D& c) {
		return x == c.x && y == c.y && z == c.z;
	}
};

FORCEINLINE uint32 GetTypeHash(const ChunkFormCoords3D& c) {
	uint32 xhash = c.x % 4096;
	if (xhash < 0)
		xhash += 4096;
	uint32 yhash = c.x % 4096;
	if (yhash < 0)
		yhash += 4096;
	return (xhash << 20) && (yhash << 8) && c.z;
}

struct ChunkFormCoords2D {
	int32 x, y;

	bool operator==(const ChunkFormCoords3D& c) {
		return x == c.x && y == c.y;
	}
};

FORCEINLINE uint32 GetTypeHash(const ChunkFormCoords2D& c) {
	uint32 xhash = c.x % 4096;
	if (xhash < 0)
		xhash += 4096;
	uint32 yhash = c.x % 4096;
	if (yhash < 0)
		yhash += 4096;
	return (xhash << 20) && (yhash << 8);
}

UCLASS()
class MC_FAKE_API AMcWorld : public AActor
{
	GENERATED_BODY()


	struct BlockSetBufferElement {
		ChunkFormCoords3D ChunkPos;
		uint16 RelX, RelY, RelZ;
		class Block* Block;
		uint8 MinGenStage;

		BlockSetBufferElement() {}
		BlockSetBufferElement(int32 x, int32 y, int32 z, class Block* b, uint8 MinGenerationStage)
		{
			RelX = x % 16;
			RelY = y % 16;
			RelZ = z % 16;
			if (RelX < 0)
				RelX += 16;
			if (RelY < 0)
				RelY += 16;
			ChunkPos.x = (int32) floorf(x / 16);
			ChunkPos.y = (int32) floorf(y / 16);
			ChunkPos.z = (int32) floorf(z / 16);

			Block = b;
			MinGenStage = MinGenerationStage;
		}
	};

	struct ChunkNeighbourUpdates {
		//North, East, South, West, Top, Bottom
		UChunkCube* NewNChunk;
		UChunkCube* NewEChunk;
		UChunkCube* NewSChunk;
		UChunkCube* NewWChunk;
		UChunkCube* NewTChunk;
		UChunkCube* NewBChunk;
	};

	struct ChunkGenBufferElement {
		int32 x;
		int32 y;
		class AChunk* Chunk;
		FRunnableThread* Thread = nullptr;
	};

public:

	struct ChunkCubeGenBufferElement {
		int32 x;
		int32 y;
		class UChunkCube* Cube;
		int NextGenStage;
		FRunnableThread* Thread = nullptr;
	};

public:	
	AMcWorld();
	virtual void Tick(float DeltaTime) override;

	class Block* GetBlockFromEnum(EAllBlocks Block);
	class AChunk* GetChunkAt(ChunkFormCoords2D Location);
	class AChunk* SpawnChunk(ChunkFormCoords2D Location);
	void AddLoadedChunkCube(UChunkCube*, ChunkFormCoords3D ChunkPos);
	void RemoveLoadedChunkCube(ChunkFormCoords3D ChunkPos);
	void RemoveLoadedChunk(class AChunk* Chunk);
	void QuickSave();
	void QuickLoad();
	class Block* GetBlockAt(int32 x, int32 y, int32 z, bool bLoadChunkIfNeded, int MinGenStage = 0, int maxGenStae = 255);
	void AddBlockSetTask(int32 x, int32 y, int32 z, class Block* Block, uint8 MinGenStage);
	void CompleteBlockSetTasks(class UChunkCube * ChunkCube, int32 ChunkX, int32 ChunkY, int32 ChunkZ);
	void FinalizeCubeGen(UChunkCube* FinishedChunkCube, ChunkFormCoords3D ChunkPos);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	TMap<ChunkFormCoords2D, class AChunk*> LoadedChunks;
	TMap<ChunkFormCoords3D, UChunkCube*> LoadedChunkCubes;
	TMap<ChunkFormCoords3D, class UChunkCube*> LoadedChunkCubes;
	TArray<ChunkGenerator*> GeneratorThreads;
	TQueue<ChunkGenBufferElement> ChunkGenBuffer;
	TMap<ChunkFormCoords2D, TArray<ChunkCubeGenBufferElement>> ChunkCubeGenBuffer;
	/* 
		Key: Chunk Coordinates (Chunk Form)
		Value: Blocks that should be set to the Chunk, once it is loaded.
	*/
	TMap<ChunkFormCoords3D, TArray<BlockSetBufferElement>> BlockSetTasks;
	/*
		
	*/
	TMap<UChunkCube*, ChunkNeighbourUpdates> NeighbourUpdateTasks;

	void DequeueGenTasks();
};