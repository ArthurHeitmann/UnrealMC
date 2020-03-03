#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChunkGenerator.h"
#include "Containers/Queue.h"
#include "Enums.h"
#include "McWorldStructs.h"
#include "ChunkFormCoords.h"
#include "Chunk.h"
#include "McWorld.generated.h"

UCLASS()
class MC_FAKE_API AMcWorld : public AActor
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditAnywhere)
	USceneComponent* ChunksRoot;

	AMcWorld();
	virtual void Tick(float DeltaTime) override;

	class Chunk* GetChunkAt(ChunkFormCoords2D Location);
	class Chunk* CreateChunk(ChunkFormCoords2D Location);
	class ChunkCube* GetChunkCubeAt(const ChunkFormCoords3D& Location);
	void AddChunkGenTask(class ChunkCube* Cube);
	void AddMeshGenTask(ChunkCube* Cube);
	void AddLoadedChunkCube(class ChunkCube*, ChunkFormCoords3D CurrChunkPos);
	void RemoveLoadedChunkCube(ChunkFormCoords3D CurrChunkPos);
	void AddRemovedChunk(class Chunk* chunk);
	void RemoveLoadedChunk(ChunkFormCoords2D Pos);
	void QuickSave();
	void QuickLoad();
	class B_Block* GetBlockAt(int32 x, int32 y, int32 z, bool bLoadChunkIfNeeded, int MinGenStage = 0, int MaxGenState = 255);
	void AddBlockSetTask(int32 x, int32 y, int32 z, class B_Block* Block, uint8 MinGenStage, bool bUpdateMesh = true);
	void CompleteBlockSetTasks(class ChunkCube * ChunkCube);
	void FinalizeChunkGen(class Chunk* Chunk);
	void FinalizeCubeGen(class ChunkCube* FinishedChunkCube, ChunkFormCoords3D CurrChunkPos);
	void DequeueChunkGenTasks();
	void DequeueChunkCubeGenTasks();
	void DequeueMeshGenTasks();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	TMap<ChunkFormCoords2D, class Chunk*> LoadedChunks;
	TMap<ChunkFormCoords3D, class ChunkCube*> LoadedChunkCubes;
	TArray<class ChunkGenerator*> GeneratorThreads;
	TArray<class ChunkMeshGeneratorThread*> MeshGeneratorThreads;
	TQueue<ChunkGenBufferElement> ChunkGenBuffer;
	TQueue<ChunkCube*> MeshGenBuffer;
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
	TMap<class ChunkCube*, ChunkNeighborUpdates> NeighborUpdateTasks;
	FCriticalSection NeighborUpdatesMutex;
	FCriticalSection ChunkCubesGenQueueMutex;

};

int32 currTick = 0;