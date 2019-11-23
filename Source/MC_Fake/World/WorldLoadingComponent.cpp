// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldLoadingComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

UWorldLoadingComponent::UWorldLoadingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bIsEnabled = true;
	ChunkLoadingDistance = 2;
	CurrentChunkCoordinates = { 1000000, 1000000 , 1000000 };
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

	FVector ChunkCoordinatesNew3D = GetComponentLocation();
	ChunkFormCoords3D ChunkCoordinatesNew{ floorf(ChunkCoordinatesNew3D.X / 1600),
		floorf(ChunkCoordinatesNew3D.Y / 1600),
		floorf(ChunkCoordinatesNew3D.Z / 1600) };
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

	FVector ChunkCoordinatesNew3D = GetComponentLocation();
	ChunkFormCoords3D ChunkCoordinatesNew { floorf(ChunkCoordinatesNew3D.X / 1600), 
		floorf(ChunkCoordinatesNew3D.Y / 1600),
		floorf(ChunkCoordinatesNew3D.Z / 1600) };

	if (CurrentChunkCoordinates == ChunkCoordinatesNew)
		return;

	ChunkFormCoords3D ChunkLocDifference = CurrentChunkCoordinates - ChunkCoordinatesNew;
	CurrentChunkCoordinates = ChunkCoordinatesNew;

	//Update life stages
	for (int i = 0; i < PlayerChunks.Num(); i++)
	{
		PlayerChunks[i] += ChunkLocDifference.To2D();
		int MaxDistance = FMath::Max(abs(PlayerChunks[i].x), abs(PlayerChunks[i].y));
		if (MaxDistance < ChunkLoadingDistance)
		{
			if (Chunk * Chunk = McFWorld->GetChunkAt({ CurrentChunkCoordinates.To2D() + PlayerChunks[i] }))
				Chunk->SetMeshLifeStage(0);
		}
		else if (MaxDistance < ChunkLoadingDistance + 3)
		{
			if (Chunk * Chunk = McFWorld->GetChunkAt({ CurrentChunkCoordinates.To2D() + PlayerChunks[i] }))
				Chunk->SetMeshLifeStage(1);
		}
		else
		{
			if (Chunk* chunk = McFWorld->GetChunkAt({ CurrentChunkCoordinates.To2D() + PlayerChunks[i] }))
			{
				PlayerChunks.Remove(CurrentChunkCoordinates.To2D() + PlayerChunks[i]);
				McFWorld->RemoveLoadedChunk(chunk);
				delete chunk;
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
					ProcessChunkDistanceUpdate({ CurrentChunkCoordinates, { x, y } });
			}
			else
			{
				ProcessChunkDistanceUpdate({ CurrentChunkCoordinates, { x, layer } });
				ProcessChunkDistanceUpdate({ CurrentChunkCoordinates, { x, -layer } });
			}
		}
	}
}

void UWorldLoadingComponent::ProcessChunkDistanceUpdate(const ChunkLoadBufferElement& ChunkPosData)
{
	if (!PlayerChunks.Contains(ChunkPosData.LocRelToCurrentChunk))
		ChunkLoadingBuffer.Enqueue(ChunkPosData);
	else
	{
		LoadChunkCubes(ChunkPosData);
	}
}

void UWorldLoadingComponent::LoadChunk(ChunkLoadBufferElement Data)
{
	//FVector ChunkCoords {  Data.x, Data.y };
	if (Chunk* NewChunk = McFWorld->CreateChunk(Data.CurrChunkPos.To2D() + Data.LocRelToCurrentChunk))
	{
		PlayerChunks.Add(Data.LocRelToCurrentChunk);

		LoadChunkCubes(Data);
	}

}

void UWorldLoadingComponent::LoadChunkCubes(ChunkLoadBufferElement Data)
{
	int8 RangeDown, RangeUp;
	CalcCubeRangeFromDist(Data, RangeDown, RangeUp);
	int8 Height = Data.CurrChunkPos.z;
	McFWorld->GetChunkAt(Data.CurrChunkPos.To2D() + Data.LocRelToCurrentChunk)
		->UpdateChunkCubesLoading(Height, RangeDown, RangeUp);
}

void UWorldLoadingComponent::CalcCubeRangeFromDist(const ChunkLoadBufferElement& ChunkPosData, int8& OutRangeDown, int8& OutRangeUp)
{
	//TODO scale with Loading Distance and environment (Cave, Hills, Structure, etc.)
	float MaxDist = fmaxf(abs(ChunkPosData.LocRelToCurrentChunk.x), abs(ChunkPosData.LocRelToCurrentChunk.y));
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
