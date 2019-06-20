// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Chunk.h"
#include "Containers/Queue.h"
#include "WorldLoadingComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MC_FAKE_API UWorldLoadingComponent : public UActorComponent
{
	struct ChunkPair { FVector2D Loc; AChunk* chunk; };

	GENERATED_BODY()

	struct ChunkLoadBufferElement { int32 x; int32 y; FVector2D RelLocation; };

private:	
	AActor* Player;
	UPROPERTY(EditAnywhere)
	int ChunkLoadingDistance;
	FVector2D CurrentChunkCoordinates;
	TArray<FVector2D> PlayerChunks;
	TQueue<ChunkLoadBufferElement> ChunkLoadingBuffer;
	class AMcWorld* McFWorld;
	UPROPERTY(EditAnywhere)
	bool bIsEnabled;

	float min4(float v1, float v2, float v3, float v4);
	void LoadChunk(ChunkLoadBufferElement Data);
	void UpdateChunkAngleVisibilities();

protected:
	virtual void BeginPlay() override;

public:	
	UWorldLoadingComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
