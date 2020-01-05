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
	bool bHasDataChanged = false;
	bool bIsGenerating = false;
	bool bHasFinishedGenerating = false;

	bool ShouldFaceBePlacedBetween(B_Block* b1, B_Block* b2, EDirection Side);
protected:

public:
	ChunkCube(ChunkFormCoords3D Pos, class AMcWorld* McWorld, Chunk* ParentChunk);
	~ChunkCube();
	void Tick(float Delta);

	ChunkFormCoords3D GetPos();
	TArray<TArray<TArray<class B_Block*>>>& GetBlockData();
	void SetNextGenerationStage(int NewStage);
	int GetNextGenerationStage();
	class Chunk* GetParentChunk();
	void SetParentChunk(class Chunk*);
	void SetHasDataChanged(bool bState = true);
	bool GetHasFinishedGenerating();
	void SetHasFinishedGenerating(bool BState = true);
	bool GetIsGenerating();
	void SetIsGenerating(bool bState);
	B_Block*& GetBlockAt(int x, int y, int z);
	ChunkCubeGenData& GetChunkCubeGenData();
	ChunkCubeNeighbors& GetChunkCubeNeighbors();
	void UpdateCubeNeighbor(EDirection NeighborSide, ChunkCube* NewNeighbor, bool bUpdateMesh = false);
	void UpdateMesh();
};
