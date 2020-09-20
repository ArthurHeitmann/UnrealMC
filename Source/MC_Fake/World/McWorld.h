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
	UPROPERTY(BlueprintReadOnly)
	int32 NumChunkGenBuffer = 0;
	UPROPERTY(BlueprintReadOnly)
	int32 NumChunkCubeGenBuffer = 0;
	UPROPERTY(BlueprintReadOnly)
	int32 NumMeshGenBuffer = 0;

	AMcWorld();
	virtual void Tick(float DeltaTime) override;

	class UChunk* GetChunkAt(FChunkFormCoords2D Location);
	class UChunk* CreateChunk(FChunkFormCoords2D Location);
	class UChunkCube* GetChunkCubeAt(const FChunkFormCoords3D& Location);
	void AddChunkGenTask(class UChunkCube* Cube);
	void AddMeshGenTask(UChunkCube* Cube);
	void AddLoadedChunkCube(class UChunkCube*, FChunkFormCoords3D CurrChunkPos);
	void RemoveLoadedChunkCube(FChunkFormCoords3D CurrChunkPos);
	void AddRemovedChunk(class UChunk* chunk);
	void RemoveLoadedChunk(FChunkFormCoords2D Pos);
	void QuickSave();
	void QuickLoad();
	class B_Block* GetBlockAt(int32 x, int32 y, int32 z, bool bLoadChunkIfNeeded, int MinGenStage = 0, int MaxGenState = 255);
	void AddBlockSetTask(int32 x, int32 y, int32 z, class B_Block* Block, uint8 MinGenStage, bool bUpdateMesh = true);
	void CompleteBlockSetTasks(class UChunkCube * ChunkCube);
	void FinalizeChunkGen(class UChunk* Chunk);
	void FinalizeCubeGen(class UChunkCube* FinishedChunkCube, FChunkFormCoords3D CurrChunkPos);
	void DequeueChunkGenTasks();
	void DequeueChunkCubeGenTasks();
	void DequeueMeshGenTasks();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	bool bHasBeenDestroyed = false;
	TMap<FChunkFormCoords2D, class UChunk*> LoadedChunks;
	TMap<FChunkFormCoords3D, class UChunkCube*> LoadedChunkCubes;
	TArray<class ChunkGenerator*> GeneratorThreads;
	TArray<class ChunkMeshGeneratorThread*> MeshGeneratorThreads;
	TQueue<ChunkGenBufferElement> ChunkGenBuffer;
	TQueue<UChunkCube*> MeshGenBuffer;
	UPROPERTY(VisibleAnywhere)
	TArray<UChunk*> RemovedChunksInGenBuffer;
	TMap<FChunkFormCoords2D, TArray<ChunkCubeGenBufferElement>> ChunkCubeGenTasks;
	TQueue<TArray<ChunkCubeGenBufferElement>> ChunkCubeGenBuffer;
	/* 
		Key: Chunk Coordinates (Chunk Form)
		Value: Blocks that should be set to the Chunk, once it is loaded.
	*/
	TMap<FChunkFormCoords3D, TArray<BlockSetBufferElement>> BlockSetTasks;
	/*
		
	*/
	TMap<class UChunkCube*, ChunkNeighborUpdates> NeighborUpdateTasks;
	FCriticalSection NeighborUpdatesMutex;
	FCriticalSection ChunkCubesGenQueueMutex;
	FCriticalSection MeshGenBufferMutex;
};

int32 currTick = 0;
