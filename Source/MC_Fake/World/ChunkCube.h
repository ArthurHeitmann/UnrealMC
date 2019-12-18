#pragma once

#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "Enums.h"
#include "McWorld.h"
//#include "McWorldStructs.h"
#include "ChunkFormCoords.h"

struct ChunkCubeGenData {
	bool bIsAboveSurface;
	bool bHasGrass;
};



struct ChunkCubeNeighbours {
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

	TArray<TArray<TArray<class Block*>>> BlockData;
	ChunkCubeGenData CubeData;
	ChunkCubeNeighbours CubeNeighbours;
	class UBoxComponent* tmpBox;
	UStaticMeshComponent* tmpStatMesh;
	class URuntimeMeshComponent* ChunkMesh;
	class URuntimeMeshComponent* CustomCollisionMesh;
	int NextGenerationStage = 0;
	//int8 Height; TODO CR
	bool bHasDataChanged = false;
	bool bIsGenerating = false;
	bool bHasFinishedGenerating = false;

	void UpdateMesh();
	bool ShouldFaceBePlacedBetween(Block* b1, Block* b2, EDirection Side);
protected:

public:
	ChunkCube(ChunkFormCoords3D Pos, class AMcWorld* McWorld, Chunk* ParentChunk);
	~ChunkCube();
	void Tick(float Delta);

	ChunkFormCoords3D GetPos();
	TArray<TArray<TArray<class Block*>>>& GetBlockData();
	void SetNextGenerationStage(int NewStage);
	int GetNextGenerationStage();
	class Chunk* GetParentChunk();
	void SetParentChunk(class Chunk*);
	//void SetHeight(int8 Height);
	void SetHasDataChanged(bool val = true);
	bool GetHasFinishedGenerating();
	void SetHasFinishedGenerating(bool val = true);
	bool GetIsGenerating();
	void SetIsGenerating(bool val);
	Block*& GetBlockAt(int x, int y, int z);
	ChunkCubeGenData& GetChunkCubeGenData();
	ChunkCubeNeighbours& GetChunkCubeNeighbours();
	void UpdateCubeNeighbour(EDirection NeighbourSide, ChunkCube* NewNeighbour, bool bUpdateMesh = false);
};
