#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChunkGenerator.h"
#include "Containers/Queue.h"
#include "Enums.h"
#include "McWorldStructs.h"
#include "ChunkFormCoords.h"
#include "Chunk.h"
//#include "Blocks/Block.h"
#include "McWorld.generated.h"

UCLASS()
class MC_FAKE_API AMcWorld : public AActor
{
	GENERATED_BODY()

public:	
	USceneComponent* ChunksRoot;

	AMcWorld();
	virtual void Tick(float DeltaTime) override;

	class Block* GetBlockFromEnum(EAllBlocks Block);
	class Chunk* GetChunkAt(ChunkFormCoords2D Location);
	class Chunk* CreateChunk(ChunkFormCoords2D Location);
	void AddChunkGenTask(class ChunkCube* Cube);
	void AddLoadedChunkCube(class ChunkCube*, ChunkFormCoords3D CurrChunkPos);
	void RemoveLoadedChunkCube(ChunkFormCoords3D CurrChunkPos);
	void AddRemovedChunk(class Chunk* chunk);
	void RemoveLoadedChunk(ChunkFormCoords2D Pos);
	void QuickSave();
	void QuickLoad();
	class Block* GetBlockAt(int32 x, int32 y, int32 z, bool bLoadChunkIfNeded, int MinGenStage = 0, int maxGenStae = 255);
	void AddBlockSetTask(int32 x, int32 y, int32 z, class Block* Block, uint8 MinGenStage);
	void CompleteBlockSetTasks(class ChunkCube * ChunkCube, int32 ChunkX, int32 ChunkY, int32 ChunkZ);
	void FinalizeChunkGen(class Chunk* Chunk);
	void FinalizeCubeGen(class ChunkCube* FinishedChunkCube, ChunkFormCoords3D CurrChunkPos);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	TMap<ChunkFormCoords2D, class Chunk*> LoadedChunks;
	TMap<ChunkFormCoords3D, class ChunkCube*> LoadedChunkCubes;
	TArray<class ChunkGenerator*> GeneratorThreads;
	TQueue<ChunkGenBufferElement> ChunkGenBuffer;
	TArray<Chunk*> RemovedChunksInGenBuffer;
	TMap<ChunkFormCoords2D, TArray<ChunkCubeGenBufferElement>> ChunkCubeGenTasks;
	TQueue<TArray<ChunkCubeGenBufferElement>> ChunkCubeGenBuffer;
	/* 
		Key: Chunk Coordinates (Chunk Form)
		Value: Blocks that should be set to the Chunk, once it is loaded.
	*/
	TMap<ChunkFormCoords3D, TArray<BlockSetBufferElement>> BlockSetTasks;
	/*
		
	*/
	TMap<class ChunkCube*, ChunkNeighbourUpdates> NeighbourUpdateTasks;
	FCriticalSection NeighbourUpdatesMutex;

	void DequeueChunkGenTasks();
	void DequeueChunkCubeGenTasks();
};