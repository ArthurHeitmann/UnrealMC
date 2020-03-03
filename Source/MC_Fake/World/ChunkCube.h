#pragma once

#include "CoreMinimal.h"
#include "Enums.h"
#include "McWorld.h"
#include "ChunkFormCoords.h"

struct ChunkCubeGenData {
	bool bIsAboveSurface;
	bool bHasGrass;
};

struct ChunkCubeNeighbors {
	ChunkCube* North = nullptr;
	ChunkCube* East = nullptr;
	ChunkCube* South = nullptr;
	ChunkCube* West = nullptr;
	ChunkCube* Top = nullptr;
	ChunkCube* Bottom = nullptr;
};

/**
 * 
 */
class MC_FAKE_API ChunkCube
{

private:
	class AMcWorld* McWorld;
	ChunkFormCoords3D Pos;
	class Chunk* ParentChunk;
	USceneComponent* Root;

	TArray<TArray<TArray<class B_Block*>>> BlockData;
	ChunkCubeGenData CubeData;
	ChunkCubeNeighbors CubeNeighbors;
	class UBoxComponent* BoundingBox;
	class URuntimeMeshComponent* ChunkMesh;
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
	
	ChunkCube(ChunkFormCoords3D Pos, class AMcWorld* McWorld, Chunk* ParentChunk);
	~ChunkCube();
	void Tick(float Delta);

	ChunkFormCoords3D GetPos();
	TArray<TArray<TArray<class B_Block*>>>& GetBlockData();
	void SetNextGenerationStage(int NewStage);
	int GetNextGenerationStage();
	class Chunk* GetParentChunk();
	void SetParentChunk(class Chunk*);
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
	void UpdateCubeNeighbor(EDirection NeighborSide, ChunkCube* NewNeighbor, bool bUpdateMesh = false);
	void UpdateMesh();
};

int32 ProcMeshGenTime = 0;
int32 ThreadInitTime = 0;