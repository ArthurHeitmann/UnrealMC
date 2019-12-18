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
		for (int i = 1; i < AllWorlds.Num(); ++i)
			AllWorlds[i]->Destroy();
	}
	else if (AllWorlds.Num() == 1)
		McFWorld = Cast<AMcWorld>(AllWorlds[0]);
	else
		McFWorld = GetWorld()->SpawnActor<AMcWorld>();

	/*FVector ChunkCoordinatesNew3D = GetComponentLocation();
	CurrentChunkCoordinates = { 
		floorf(ChunkCoordinatesNew3D.X / 1600),
		floorf(ChunkCoordinatesNew3D.Y / 1600),
		floorf(ChunkCoordinatesNew3D.Z / 1600) 
	};			TODO CR  */

}

void UWorldLoadingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsEnabled)
		return;

	FVector ChunkCoordinatesNew3D = GetComponentLocation();
	ChunkFormCoords3D ChunkCoordinatesNew { floorf(ChunkCoordinatesNew3D.X / 1600), 
		floorf(ChunkCoordinatesNew3D.Y / 1600),
		floorf(ChunkCoordinatesNew3D.Z / 1600) };

	if (CurrentChunkCoordinates == ChunkCoordinatesNew)
		return;
	UE_LOG(LogTemp, Warning, TEXT("Position changed \n X: %d Y: %d Z: %d"), ChunkCoordinatesNew.x, ChunkCoordinatesNew.y, ChunkCoordinatesNew.z);
	//ChunkFormCoords3D ChunkLocDifference = ChunkCoordinatesNew - CurrentChunkCoordinates;		TODO CR
	CurrentChunkCoordinates = ChunkCoordinatesNew;
	ChunkFormCoords2D ChunkCoords2D = CurrentChunkCoordinates.To2D();

	//Update life stages
	for (int i = 0; i < PlayerChunks.Num(); ++i)
	{
		ChunkFormCoords2D LocDiff = ChunkCoords2D - PlayerChunks[i];
		//PlayerChunks[i] += LocDiff;		TODO CR
		int MaxDistance = FMath::Max(abs(LocDiff.x), abs(LocDiff.y));
		if (MaxDistance < ChunkLoadingDistance + 3)
		{
			if (Chunk* Chunk = McFWorld->GetChunkAt(PlayerChunks[i]))
				Chunk->SetMeshLifeStage(0);
		}
		else
		{
			if (Chunk* chunk = McFWorld->GetChunkAt(PlayerChunks[i]))
			{
				ChunkFormCoords2D tmpCoords = PlayerChunks[i];
				PlayerChunks.Remove(tmpCoords);
				delete chunk;
			}
		}
	}

	//Enqueue new chunks, that are now within the render distance/ChunkLoadingDistance   
	for (int layer = 0; layer <= ChunkLoadingDistance - 1; ++layer)
	{
		for (int x = -layer; x <= layer; ++x)
		{
			if (abs(x) == layer)
			{
				for (int y = -layer; y <= layer; ++y)
					ProcessChunkDistanceUpdate({ ChunkCoords2D + ChunkFormCoords2D{ x, y }, CurrentChunkCoordinates.z });
			}
			else
			{
				ProcessChunkDistanceUpdate({ ChunkCoords2D + ChunkFormCoords2D{ x, layer }, CurrentChunkCoordinates.z });
				ProcessChunkDistanceUpdate({ ChunkCoords2D + ChunkFormCoords2D{ x, -layer }, CurrentChunkCoordinates.z });
			}
		}
	}
}

void UWorldLoadingComponent::ProcessChunkDistanceUpdate(const ChunkFormCoords3D & Pos3D)
{
	if (!PlayerChunks.Contains(Pos3D.To2D()))
		LoadChunk(Pos3D);
	else
	{
		LoadChunkCubes(Pos3D);
	}
}

void UWorldLoadingComponent::LoadChunk(ChunkFormCoords3D Pos3D)
{
	//FVector ChunkCoords {  Data.x, Data.y };
	if (Chunk* NewChunk = McFWorld->CreateChunk(Pos3D.To2D()))
	{
		PlayerChunks.Add(Pos3D.To2D());

		LoadChunkCubes(Pos3D);
	}

}

void UWorldLoadingComponent::LoadChunkCubes(ChunkFormCoords3D Pos3D)
{
	int8 RangeDown, RangeUp;
	CalcCubeRangeFromDist(Pos3D.To2D(), RangeDown, RangeUp);
	int8 Height = Pos3D.z;
	McFWorld->GetChunkAt(Pos3D.To2D())
		->UpdateChunkCubesLoading(Height, RangeDown, RangeUp);
}

void UWorldLoadingComponent::CalcCubeRangeFromDist(const ChunkFormCoords2D& Pos2D, int8& OutRangeDown, int8& OutRangeUp)
{
	//TODO scale with Loading Distance and environment (Cave, Hills, Structure, etc.)
	ChunkFormCoords2D LocDiff = CurrentChunkCoordinates.To2D() - Pos2D;
	float MaxDist = fmaxf(abs(LocDiff.x), abs(LocDiff.y));
	if (MaxDist <= 5)
	{
		OutRangeDown = 2;
		OutRangeUp = 4;
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
