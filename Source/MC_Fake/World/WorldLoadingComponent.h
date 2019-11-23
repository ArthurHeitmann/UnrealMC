// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Chunk.h"
#include "Containers/Queue.h"
#include "McWorld.h"
#include "ChunkFormCoords.h"
//#include "McWorldStructs.h"
#include "WorldLoadingComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MC_FAKE_API UWorldLoadingComponent : public USceneComponent
{
	//struct ChunkPair { FVector2D Loc; Chunk* chunk; };

	GENERATED_BODY()

	struct ChunkLoadBufferElement { ChunkFormCoords3D CurrChunkPos ; ChunkFormCoords2D LocRelToCurrentChunk; };

private:
	UPROPERTY(EditAnywhere)
	int ChunkLoadingDistance;
	/* Absolute coordinates (in Chunk Form) where the player/component currently is */
	ChunkFormCoords3D CurrentChunkCoordinates;
	TArray<ChunkFormCoords2D> PlayerChunks;
	TQueue<ChunkLoadBufferElement> ChunkLoadingBuffer;
	class AMcWorld* McFWorld;
	UPROPERTY(EditAnywhere)
	bool bIsEnabled;

	/**
	 Input: Absolute Coordinates (Chunk form) of new chunk
	 If the Chunk doesn't exist yet it gets added to the ChunkBuffer Queue
	*/
	void ProcessChunkDistanceUpdate(const ChunkLoadBufferElement& ChunkPosData);
	void LoadChunk(ChunkLoadBufferElement Data);
	void LoadChunkCubes(ChunkLoadBufferElement Data);
	/*  */
	void CalcCubeRangeFromDist(const ChunkLoadBufferElement& ChunkPosData, int8& OutRangeDown, int8& OutRangeUp);

protected:
	virtual void BeginPlay() override;

public:	
	UWorldLoadingComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
