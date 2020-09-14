#include "LineTracingInteractions.h"
#include "RuntimeMeshComponent.h"
#include "Engine/World.h"
#include "World/ChunkCube.h"
#include "Blocks/BlockBreaking.h"
#include "Camera/CameraComponent.h"
#include "Items/I_NoItem.h"
#include "World/McWorld.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/McStaticFunctions.h"
#include "Blocks/B_Block.h"
#include "Blocks/BlockManager.h"
#include "World/ChunkMeshGenerator.h"

ULineTracingInteractions::ULineTracingInteractions()
{
	PrimaryComponentTick.bCanEverTick = true;

	HandReachDistance = 500;
}


void ULineTracingInteractions::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> FoundWorlds;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMcWorld::StaticClass(), FoundWorlds);
	if (FoundWorlds.Num())
		McWorld = Cast<AMcWorld>(FoundWorlds[0]);
}


void ULineTracingInteractions::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (bIsHitting)
		LeftClickStart();
}

void ULineTracingInteractions::LeftClickStart()
{
	bIsHitting = true;
	if (!RayShootingCamera)
		return;
	FVector Start(RayShootingCamera->GetComponentLocation());
	FVector Direction = RayShootingCamera->GetComponentTransform().GetRotation().GetForwardVector();
	FHitResult Result;
	FVector EndPosition(Start + Direction * HandReachDistance);
	if (GetWorld()->LineTraceSingleByChannel(Result, Start, EndPosition, ECC_Visibility))
	{
		if (Cast<URuntimeMeshComponent>(Result.Component))
		{
			FChunkFormCoords3D TmpChunkPos;
			FIntVector TmpRelPos;
			CalcChunkAndRelBlockCoords(Result, TmpChunkPos, TmpRelPos);
			if (BreakingIndicator)
			{
				if (TmpChunkPos != ChunkPos || TmpRelPos != RelPos)
				{
					CancelBreaking();
					ChunkPos = TmpChunkPos;
					RelPos = TmpRelPos;
					RegisterNewHit(Result);
				}
				else
				{
					ContinueBreaking();
				}
			}
			else
			{
				ChunkPos = TmpChunkPos;
				RelPos = TmpRelPos;
				RegisterNewHit(Result);
			}

			/*if (ABlockBreaking* BreakingBlock = Cast<ABlockBreaking>(Result.Actor))
			{
				if (BreakingBlock != BreakingIndicator)
				{
					CancelBreaking();
					RegisterNewHit();
				}
				else
					ContinueBreaking();
			}
			else if (AMcWorld* Block = Cast<AMcWorld>(Result.Actor))
			{
				if (BreakingIndicator)
					CancelBreaking();

				RegisterNewHit();
			}*/
		}
	}
	else if (BreakingIndicator)
	{
		CancelBreaking();
	}
}

void ULineTracingInteractions::RegisterNewHit(const FHitResult& Hit)
{
	int32 RelX = RelPos[0];
	int32 RelY = RelPos[1];
	int32 RelZ = RelPos[2];
	
	HittingCCube = McWorld->GetChunkCubeAt(ChunkPos);
	checkf(HittingCCube, TEXT("Hitting Coordinates lead to nonexistent chunk cube!"));
	HittingBlock = HittingCCube->GetBlockData()[RelX][RelY][RelZ];

	if (HittingBlock && HittingBlock->GetName() == "Air")
	{
		UE_LOG(LogTemp, Error, TEXT("Hitting Coordinates lead to nonexistent Block or Air!"));
		HittingBlock = nullptr;
		HittingCCube = nullptr;
		return;
	}
	
	InitialBlock = &(HittingCCube->GetBlockData()[RelX][RelY][RelZ]);
	HittingCCube->GetBlockData()[RelX][RelY][RelZ] = BlockManager::GetStaticBlock("Air");
	HittingBlockCoordinates = ChunkPos.ToWorldCoordinates() + FVector(RelX, RelY, RelZ) * 100.f;
	BreakingIndicator = GetWorld()->SpawnActor<ABlockBreaking>(HittingBlockCoordinates, FRotator::ZeroRotator, FActorSpawnParameters());
	BreakingIndicator->InitWithBlock(HittingBlock);

	float speedFactor = 1;
	if ((*CurrentItem)->ItemS->GetItemActions().Contains(HittingBlock->GetBreakingAction()))
		speedFactor = (*CurrentItem)->ItemS->GetBreakingSpeed();
	TimeSpentHitting = GetWorld()->GetDeltaSeconds() * speedFactor;
	BreakingIndicator->SetTimePassed(TimeSpentHitting);

	HittingBlock->OnBeginBreak(GetWorld(), Hit.ImpactPoint);

	LastUsedItem = (*CurrentItem)->ItemS;

	auto NeighborCCubes = HittingCCube->GetChunkCubeNeighbors();
	if (RelX == 0 && NeighborCCubes.South)
		NeighborCCubes.South->SetHasBlockDataChanged();
	if (RelX == 15 && NeighborCCubes.North)
		NeighborCCubes.North->SetHasBlockDataChanged();
	if (RelY == 0 && NeighborCCubes.West)
		NeighborCCubes.West->SetHasBlockDataChanged();
	if (RelY == 15 && NeighborCCubes.East)
		NeighborCCubes.East->SetHasBlockDataChanged();
	if (RelZ == 0 && NeighborCCubes.Bottom)
		NeighborCCubes.Bottom->SetHasBlockDataChanged();
	if (RelZ == 15 && NeighborCCubes.Top)
		NeighborCCubes.Top->SetHasBlockDataChanged();

	HittingCCube->SetHasBlockDataChanged();
}

void ULineTracingInteractions::ContinueBreaking()
{
	if (LastUsedItem != (*CurrentItem)->ItemS)
	{
		LastUsedItem = (*CurrentItem)->ItemS;
		TimeSpentHitting = 0.f;
	}
	
	float speedFactor = 1;
	if ((*CurrentItem)->ItemS->GetItemActions().Contains(HittingBlock->GetBreakingAction()))
		speedFactor = (*CurrentItem)->ItemS->GetBreakingSpeed();
	TimeSpentHitting += GetWorld()->GetDeltaSeconds() * speedFactor;
	BreakingIndicator->SetTimePassed(TimeSpentHitting);

	if (TimeSpentHitting >= HittingBlock->GetBreakTime())
	{
		HittingBlock->OnBreak(GetWorld(), HittingBlockCoordinates);
		BreakingIndicator->Destroy();
		BreakingIndicator = nullptr;
		HittingBlock = nullptr;
		InitialBlock = nullptr;
	}

	//technically you should update the cube neighbors here (like in RegisterHitAt()).
	//This would cause a very small lag. Now there might be up to 6 redundant faces,
	//which is balances the con out.
}

void ULineTracingInteractions::CancelBreaking()
{
	
	(*InitialBlock) = HittingBlock;
	ChunkMeshGenerator::GenerateChunkMesh(HittingCCube);
	HittingCCube->UpdateMesh();
	BreakingIndicator->Destroy();
	BreakingIndicator = nullptr;
	HittingBlock = nullptr;
	InitialBlock = nullptr;
}

void ULineTracingInteractions::CalcChunkAndRelBlockCoords(const FHitResult& Hit, FChunkFormCoords3D& chunkPosOut,
	FIntVector& RelOut)
{
	FVector HitLoc = Hit.ImpactPoint / 100.f;
	HitLoc.X = HitHelper(Hit.Normal.X, HitLoc.X);
	HitLoc.Y = HitHelper(Hit.Normal.Y, HitLoc.Y);
	HitLoc.Z = HitHelper(Hit.Normal.Z, HitLoc.Z);
	chunkPosOut = FChunkFormCoords3D::FromNormalCoords(HitLoc.X, HitLoc.Y, HitLoc.Z);
	RelOut.X = McStaticFunctions::SmartMod(HitLoc.X, 16);
	RelOut.Y = McStaticFunctions::SmartMod(HitLoc.Y, 16);
	RelOut.Z = McStaticFunctions::SmartMod(HitLoc.Z, 16);
	
}

float ULineTracingInteractions::HitHelper(float NormalAxisVal, float Val)
{
	if (NormalAxisVal == 0.f)
		return floorf(Val);
	else
	{
		float Rounded = roundf(Val);
		if (NormalAxisVal == 1.f)
			return Rounded > Val ? floorf(Val) : floorf(Val) - 1;
		else if (NormalAxisVal == -1.f)
			return Rounded;
		else
			throw "Invalid Normal! Must be 0.0 or 1.0 or -1.0";
	}
}

void ULineTracingInteractions::LeftClickEnd()
{
	bIsHitting = false;
	
	if (BreakingIndicator)
		CancelBreaking();

	BreakingIndicator = nullptr;
	HittingBlock = nullptr;
}

void ULineTracingInteractions::RightClickStart()
{
	if (!RayShootingCamera)
		return;
	FVector Start(RayShootingCamera->GetComponentLocation());
	FVector Direction = RayShootingCamera->GetComponentTransform().GetRotation().GetForwardVector();
	FHitResult Result;
	FVector EndPosition(Start + Direction * HandReachDistance);
	if (GetWorld()->LineTraceSingleByChannel(Result, Start, EndPosition, ECC_Visibility))
	{
		I_Item::PostUseTask Task = (*CurrentItem)->ItemS->OnItemUse(Result, McWorld);
		switch (Task.Tasks) {
		case Decrement:
			(*CurrentItem)->ItemCount -= Task.Count;
			if ((*CurrentItem)->ItemCount <= 0)
			{
				delete (*CurrentItem)->ItemS;
				(*CurrentItem)->ItemS = new I_NoItem();
				(*CurrentItem)->ItemCount = 0;
			}
		}
		
	}
}

void ULineTracingInteractions::RightClickEnd()
{
}

void ULineTracingInteractions::SetCamera(UCameraComponent* Cam)
{
	RayShootingCamera = Cam;
}

void ULineTracingInteractions::SetSelectedItemPointer(FItemStack ** NewPointer)
{
	CurrentItem = NewPointer;
}
