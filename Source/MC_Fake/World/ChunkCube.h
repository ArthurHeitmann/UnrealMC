#pragma once

#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "Enums.h"
#include "McWorld.h"
#include "ChunkCube.generated.h"

struct ChunkCubeGenData {
	bool bIsAboveSurface;
	bool bHasGrass;
};


struct ChunkCubeNeighbours {
	UChunkCube *North, *East, *South, *West, *Top, *Bottom;
};

/**
 * 
 */
UCLASS()
class MC_FAKE_API UChunkCube : public UPrimitiveComponent
{
	GENERATED_BODY()

private:
	ChunkFormCoords3D Pos;
	TArray<TArray<TArray<class Block*>>> BlockData;
	ChunkCubeGenData CubeData;
	ChunkCubeNeighbours CubeNeighbours;
	UPROPERTY(EditAnywhere)
	class URuntimeMeshComponent* ChunkMesh;
	UPROPERTY(EditAnywhere)
	class URuntimeMeshComponent* CustomCollisionMesh;
	int NextGenerationStage = 0;
	class AChunk* ParentChunk;
	int8 Height;
	bool bHasDataChanged;

	void UpdateMesh();
	bool ShouldFaceBePlacedBetween(Block* b1, Block* b2, EDirection Side);
protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float Delta, ELevelTick Type, FActorComponentTickFunction* TickFunction) override;
	virtual void EndPlay(EEndPlayReason::Type Reason) override;

public:
	UChunkCube();

	ChunkFormCoords3D GetPos();
	TArray<TArray<TArray<class Block*>>>& GetBlockData();
	void SetNextGenerationStage(int NewStage);
	int GetNextGenerationStage();
	void SetParentChunk(class AChunk*);
	void SetHeight(int8 Height);
	void SetHasDataChanged(bool val = true);
	Block*& GetBlockAt(int x, int y, int z);
	ChunkCubeGenData& GetChunkCubeGenData();
	ChunkCubeNeighbours& GetChunkCubeNeighbours();
	void UpdateCubeNeighbour(EDirection NeighbourSide, UChunkCube* NewNeighbour, bool bUpdateMesh = false);
};
