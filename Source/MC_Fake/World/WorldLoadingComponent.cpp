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

	//UpdateChunkAngleVisibilities();

	FVector ChunkCoordinatesNew3D = Player->GetActorLocation();
	FVector2D ChunkCoordinatesNew(ChunkCoordinatesNew3D.X, ChunkCoordinatesNew3D.Y);
	ChunkCoordinatesNew.X = FGenericPlatformMath::FloorToInt(ChunkCoordinatesNew.X / 1600);
	ChunkCoordinatesNew.Y = FGenericPlatformMath::FloorToInt(ChunkCoordinatesNew.Y / 1600);
	/*if (ChunkCoordinatesNew3D.X < 0)
		ChunkCoordinatesNew.X -= 1;
	if (ChunkCoordinatesNew3D.Y < 0)
		ChunkCoordinatesNew.Y -= 1;*/

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

	for (int layer = 0; layer <= ChunkLoadingDistance - 1; layer++)
	{
		for (int x = -layer; x <= layer; x++)
		{
			if (abs(x) == layer)
			{
				for (int y = -layer; y <= layer; y++)
					ChunkLoadingBuffer.Enqueue({ x, y, CurrentChunkCoordinates });
			}
			else
			{
				ChunkLoadingBuffer.Enqueue({ x, layer, CurrentChunkCoordinates });
				ChunkLoadingBuffer.Enqueue({ x, -layer, CurrentChunkCoordinates });
			}
		}
	}
}

void UWorldLoadingComponent::LoadChunk(ChunkLoadBufferElement Data)
{
	if (AChunk * NewChunk = McFWorld->SpawnChunk({ (Data.x + Data.RelLocation.X) * 1600.f, (Data.y + Data.RelLocation.Y) * 1600.f }))
		PlayerChunks.Add({(float) Data.x, (float) Data.y});
}

void UWorldLoadingComponent::UpdateChunkAngleVisibilities()
{
	if (APawn * PawnOwner = Cast<APawn>(GetOwner()))
	{
		float ControllerYaw = FMath::DegreesToRadians(PawnOwner->GetControlRotation().Yaw);
		FVector2D ForwardVec(cos(ControllerYaw), sin(ControllerYaw));
		for (FVector2D RelLoc : PlayerChunks)
		{
			AChunk* Chunk = McFWorld->GetChunkAt(CurrentChunkCoordinates + RelLoc);
			if (!Chunk)
				continue;

			if (!RelLoc.SizeSquared() || RelLoc.SizeSquared() < 2.1)
			{
				Chunk->SetMeshLifeStage(0);
				continue;
			}
			FVector2D RelLoc1 = RelLoc;
			FVector2D RelLoc2(RelLoc.X + 1, RelLoc.Y);
			FVector2D RelLoc3(RelLoc.X, RelLoc.Y + 1);
			FVector2D RelLoc4(RelLoc.X + 1, RelLoc.Y + 1);
			float angle1 = FMath::Acos((RelLoc1.X * ForwardVec.X + RelLoc1.Y * ForwardVec.Y) / (RelLoc1.Size() * ForwardVec.Size()));
			float angle2 = FMath::Acos((RelLoc2.X * ForwardVec.X + RelLoc2.Y * ForwardVec.Y) / (RelLoc2.Size() * ForwardVec.Size()));
			float angle3 = FMath::Acos((RelLoc3.X * ForwardVec.X + RelLoc3.Y * ForwardVec.Y) / (RelLoc3.Size() * ForwardVec.Size()));
			float angle4 = FMath::Acos((RelLoc4.X * ForwardVec.X + RelLoc4.Y * ForwardVec.Y) / (RelLoc4.Size() * ForwardVec.Size()));
			float MinAngle = min4(angle1, angle2, angle3, angle4);

			if (MinAngle < FMath::DegreesToRadians(90))			//TODO change to actual fov angle
			{
				Chunk->SetMeshLifeStage(0);
			}
			else
			{
				Chunk->SetMeshLifeStage(1);
			}
		}
	}
}



float UWorldLoadingComponent::min4(float v1, float v2, float v3, float v4)
{
	
	return FMath::Min(FMath::Min(v1, v2), FMath::Min(v3, v4));
}
