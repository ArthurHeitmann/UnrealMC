#include "WorldLoadingComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/DateTime.h"

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
}

void UWorldLoadingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsEnabled)
		return;

	FVector ChunkCoordinatesNew3D = GetComponentLocation();
	FChunkFormCoords3D ChunkCoordinatesNew { floorf(ChunkCoordinatesNew3D.X / 1600), 
		floorf(ChunkCoordinatesNew3D.Y / 1600),
		floorf(ChunkCoordinatesNew3D.Z / 1600) };

	if (CurrentChunkCoordinates == ChunkCoordinatesNew)
		return;

	int32 t1 = FDateTime::Now().GetMillisecond();
	
	CurrentChunkCoordinates = ChunkCoordinatesNew;
	FChunkFormCoords2D ChunkCoords2D = CurrentChunkCoordinates.To2D();

	//Update life stages
	for (int i = 0; i < PlayerChunks.Num(); ++i)
	{
		FChunkFormCoords2D LocDiff = ChunkCoords2D - PlayerChunks[i];
		// if (FMath::Max(abs(LocDiff.X), abs(LocDiff.Y)) > ChunkLoadingDistance - 1)
		if (FMath::Max(abs(LocDiff.X), abs(LocDiff.Y)) > ChunkLoadingDistance + 3)
		{
			if (UChunk* chunk = McFWorld->GetChunkAt(PlayerChunks[i]))
			{
				PlayerChunks.RemoveAt(i--);               //i-- so that the index doesn't change on the next loop iteration
				chunk->DestroyComponent();
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
					ProcessChunkDistanceUpdate({ ChunkCoords2D + FChunkFormCoords2D{ x, y }, CurrentChunkCoordinates.Z });
			}
			else
			{
				ProcessChunkDistanceUpdate({ ChunkCoords2D + FChunkFormCoords2D{ x, layer }, CurrentChunkCoordinates.Z });
				ProcessChunkDistanceUpdate({ ChunkCoords2D + FChunkFormCoords2D{ x, -layer }, CurrentChunkCoordinates.Z });
			}
		}
	}


	int32 t2 = FDateTime::Now().GetMillisecond();
	UE_LOG(LogTemp, Warning, TEXT("WorldLoadingComp tick time: %d"), t2 - t1);
	
}

void UWorldLoadingComponent::ResetWorld()
{
	for (int i = 0; i < PlayerChunks.Num(); ++i)
	{
		if (UChunk* chunk = McFWorld->GetChunkAt(PlayerChunks[i]))
		{
			PlayerChunks.RemoveAt(i--);               //i-- so that the index doesn't change on the next loop iteration
			delete chunk;
		}
	}
	
	CurrentChunkCoordinates = { 1000000, 1000000 , 1000000 };
}

void UWorldLoadingComponent::ProcessChunkDistanceUpdate(const FChunkFormCoords3D & Pos3D)
{
	if (!PlayerChunks.Contains(Pos3D.To2D()))
		LoadChunk(Pos3D);
	else
		LoadChunkCubes(Pos3D);
}

void UWorldLoadingComponent::LoadChunk(FChunkFormCoords3D Pos3D)
{
	if (McFWorld->CreateChunk(Pos3D.To2D()))
	{
		PlayerChunks.Add(Pos3D.To2D());
		LoadChunkCubes(Pos3D);
	}

}

void UWorldLoadingComponent::LoadChunkCubes(FChunkFormCoords3D Pos3D)
{
	int8 RangeDown, RangeUp;
	CalcCubeRangeFromDist(Pos3D.To2D(), RangeDown, RangeUp);
	int8 Height = Pos3D.Z;
	McFWorld->GetChunkAt(Pos3D.To2D())
		->UpdateChunkCubesLoading(Height, RangeDown, RangeUp);
}

void UWorldLoadingComponent::CalcCubeRangeFromDist(const FChunkFormCoords2D& Pos2D, int8& OutRangeDown, int8& OutRangeUp)
{
	//TODO scale with Loading Distance and environment (Cave, Hills, Structure, etc.)
	FChunkFormCoords2D LocDiff = CurrentChunkCoordinates.To2D() - Pos2D;
	float MaxDist = fmaxf(abs(LocDiff.X), abs(LocDiff.Y));
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
