#pragma once

#include "CoreMinimal.h"
#include "Enums.h"
#include "McWorld.h"
#include "ChunkFormCoords.h"
#include "ChunkCube.generated.h"

struct ChunkCubeGenData {
	bool bIsAboveSurface;
	bool bHasGrass;
};

struct ChunkCubeNeighbors {
	UChunkCube* North = nullptr;
	UChunkCube* East = nullptr;
	UChunkCube* South = nullptr;
	UChunkCube* West = nullptr;
	UChunkCube* Top = nullptr;
	UChunkCube* Bottom = nullptr;
};

UCLASS()
class MC_FAKE_API UChunkCube : public USceneComponent
{
GENERATED_BODY()
	
private:
	bool bHasBeenDestroyed = false;
	UPROPERTY()
	class AMcWorld* McWorld;
	FChunkFormCoords3D Pos;
	UPROPERTY()
	class UChunk* ParentChunk;

	TArray<TArray<TArray<class B_Block*>>> BlockData;
	ChunkCubeGenData CubeData;
	ChunkCubeNeighbors CubeNeighbors;
	UPROPERTY()
	class UBoxComponent* BoundingBox;
	UPROPERTY()
	class URuntimeMeshComponent* ChunkMesh;
	UPROPERTY()
	class URuntimeMeshComponent* CustomCollisionMesh;
	int NextGenerationStage = 0;
	bool bHasMeshDataChanged = false;
	bool bHasBlockDataChanged = false;
	bool bIsGenerating = false;
	bool bHasFinishedGenerating = false;
	bool bIsMeshGenPending = false;

protected:

public:
	TMap<uint16, TArray<FVector>> Vertices;
	TMap<uint16, TArray<FVector2D>> UVs;
	TMap<uint16, TArray<int32>> Triangles;
	TMap<uint16, TArray<FVector>> Normals;
	TMap<uint16, B_Block*> Materials;
	TMap<uint16, TArray<FVector>> VerticesCustomCollision;
	TMap<uint16, TArray<int32>> TrianglesCustomCollision;
	FCriticalSection MeshLock;

	UChunkCube();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void Init(FChunkFormCoords3D Pos, class AMcWorld* McWorld, UChunk* ParentChunk);
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	FChunkFormCoords3D GetPos();
	TArray<TArray<TArray<class B_Block*>>>& GetBlockData();
	void SetNextGenerationStage(int NewStage);
	int GetNextGenerationStage();
	class UChunk* GetParentChunk();
	void SetParentChunk(class UChunk*);
	void SetHasMeshDataChanged(bool bState = true);
	void SetHasBlockDataChanged(bool bState = true);
	bool GetHasFinishedGenerating();
	void SetHasFinishedGenerating(bool BState = true);
	void SetIsMeshGenPending(bool val);
	bool GetIsGenerating();
	void SetIsGenerating(bool bState);
	B_Block*& GetBlockAt(int x, int y, int z);
	ChunkCubeGenData& GetChunkCubeGenData();
	ChunkCubeNeighbors& GetChunkCubeNeighbors();
	void UpdateCubeNeighbor(EDirection NeighborSide, UChunkCube* NewNeighbor, bool bUpdateMesh = false);
	void UpdateMesh();
	bool HasBeenDestroyed();
};

int32 ProcMeshGenTime = 0;
int32 ThreadInitTime = 0;