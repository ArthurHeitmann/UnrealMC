// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldLoadingComponent.h"
#include "GameFramework/Actor.h"
#include "WorldLoader.h"
#include "McWorld.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

UWorldLoadingComponent::UWorldLoadingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bIsEnabled = true;
	Player = GetOwner();
	ChunkLoadingDistance = 2;
	CurrentChunkCoordinates = { .5f, .5f };
}

void UWorldLoadingComponent::BeginPlay()
{
	Super::BeginPlay();
	TArray<AActor*> AllWorlds;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), TSubclassOf<AMcWorld>(AMcWorld::StaticClass()), AllWorlds);
	if (AllWorlds.Num() > 1)
	{
		for (int i = 1; i < AllWorlds.Num(); i++)
			AllWorlds[i]->Destroy();
	}
	else if (AllWorlds.Num() == 1)
		McFWorld = Cast<AMcWorld>(AllWorlds[0]);
	else
		McFWorld = GetWorld()->SpawnActor<AMcWorld>();

}

void UWorldLoadingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsEnabled)
		return;

	if (!ChunkLoadingBuffer.IsEmpty())
	{
		ChunkLoadBufferElement NewChunk;
		ChunkLoadingBuffer.Dequeue(NewChunk);
		LoadChunk(NewChunk);
	}

	FVector ChunkCoordinatesNew3D = Player->GetActorLocation();
	FVector2D ChunkCoordinatesNew(ChunkCoordinatesNew3D.X, ChunkCoordinatesNew3D.Y);
	ChunkCoordinatesNew.X = FGenericPlatformMath::FloorToInt(ChunkCoordinatesNew.X / 1600);
	ChunkCoordinatesNew.Y = FGenericPlatformMath::FloorToInt(ChunkCoordinatesNew.Y / 1600);

	if (CurrentChunkCoordinates == ChunkCoordinatesNew)
		return;

	FVector2D ChunkLocDifference = CurrentChunkCoordinates - ChunkCoordinatesNew;
	CurrentChunkCoordinates = ChunkCoordinatesNew;

	//Update life stages
	for (int i = 0; i < PlayerChunks.Num(); i++)
	{
		PlayerChunks[i] += ChunkLocDifference;
		int MaxDistance = FMath::Max(abs(PlayerChunks[i].X), abs(PlayerChunks[i].Y));
		if (MaxDistance < ChunkLoadingDistance)
		{
			if (AChunk * Chunk = McFWorld->GetChunkAt({ CurrentChunkCoordinates + PlayerChunks[i] }))
				Chunk->SetMeshLifeStage(0);
		}
		else if (MaxDistance < ChunkLoadingDistance + 2)
		{
			if (AChunk * Chunk = McFWorld->GetChunkAt({ CurrentChunkCoordinates + PlayerChunks[i] }))
				Chunk->SetMeshLifeStage(1);
		}
		else
		{
			if (AChunk * Chunk = McFWorld->GetChunkAt({ CurrentChunkCoordinates + PlayerChunks[i] }))
			{
				PlayerChunks.Remove(CurrentChunkCoordinates + PlayerChunks[i]);
				McFWorld->RemoveLoadedChunk(Chunk);
				Chunk->Destroy();
			}
		}
	}

	//Enqueue new chunks, that are now within the render distance/ChunkLoadingDistance   
	for (int layer = 0; layer <= ChunkLoadingDistance - 1; layer++)
	{
		for (int x = -layer; x <= layer; x++)
		{
			if (abs(x) == layer)
			{
				for (int y = -layer; y <= layer; y++)
					ProcessChunkDistanceUpdate({ x, y, CurrentChunkCoordinates });
			}
			else
			{
				ProcessChunkDistanceUpdate({ x, layer, CurrentChunkCoordinates });
				ProcessChunkDistanceUpdate({ x, -layer, CurrentChunkCoordinates });
			}
		}
	}
}

void UWorldLoadingComponent::ProcessChunkDistanceUpdate(const ChunkLoadBufferElement& ChunkPosData)
{
	AMcWorld::ChunkFormCoords2D ChunkAbsLoc = AMcWorld::ChunkFormCoords2D{ (float) ChunkPosData.x, (float) ChunkPosData.y };
	if (!PlayerChunks.Contains(ChunkAbsLoc))
		ChunkLoadingBuffer.Enqueue(ChunkPosData);
	else
	{
		int8 RangeDown, RangeUp;
		CalcCubeRangeFromDist(ChunkPosData, RangeDown, RangeUp);
		int8 Height = (int8) floorf(Player->GetActorLocation().Z);

		McFWorld->GetChunkAt(ChunkAbsLoc)
			->UpdateChunkCubesLoading(Height, RangeDown, RangeUp);
	}
}

void UWorldLoadingComponent::LoadChunk(ChunkLoadBufferElement Data)
{
	FVector2D ChunkCoords = { (float) Data.x, (float) Data.y };
	if (AChunk * NewChunk = McFWorld->SpawnChunk({ (Data.x + Data.RelLocation.X), (Data.y + Data.RelLocation.Y) }))
		PlayerChunks.Add(ChunkCoords);
}

void UWorldLoadingComponent::CalcCubeRangeFromDist(const ChunkLoadBufferElement& ChunkPosData, int8& OutRangeDown, int8& OutRangeUp)
{
	//TODO scale with Loading Distance and environment (Cave, Hills, Structure, etc.)
	float MaxDist = fmaxf(ChunkPosData.x, ChunkPosData.y);
	if (MaxDist <= 5)
	{
		OutRangeDown = 2;
		OutRangeUp = 16;
	}
	else
	{
		OutRangeDown = 1;

		if (MaxDist <= 9)
			OutRangeUp = 3;
		else
			OutRangeUp = 2;
	}
}
