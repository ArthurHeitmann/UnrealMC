#include "LineTraceingInteractions.h"
#include "ProceduralMeshComponent/Public/ProceduralMeshComponent.h"
#include "RuntimeMeshComponent.h"
#include "Engine/World.h"
#include "World/Chunk.h"
#include "Blocks/BlockBreaking.h"
#include "Camera/CameraComponent.h"
#include "Items/I_NoItem.h"
#include "World/McWorld.h"
#include "Kismet/GameplayStatics.h"
#include "Blocks/B_Dirt.h"

// Sets default values for this component's properties
ULineTraceingInteractions::ULineTraceingInteractions()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	HandReachDistance = 500;
}


// Called when the game starts
void ULineTraceingInteractions::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> FoundWorlds;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMcWorld::StaticClass(), FoundWorlds);
	if (FoundWorlds.Num())
		World = Cast<AMcWorld>(FoundWorlds[0]);
}


// Called every frame
void ULineTraceingInteractions::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsHitting)
		LeftClickStart();
}

void ULineTraceingInteractions::LeftClickStart()
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
			if (AChunk * Chunk = Cast<AChunk>(Result.Actor))
			{
				Block* NewHittingBlock = Chunk->RegisterHitAt(Result, (*CurrentItem)->ItemS);
				if (HittingBlock && NewHittingBlock != HittingBlock)
					HittingChunk->CancelBreaking(HittingBlock);
				HittingChunk = Chunk;
				HittingBlock = NewHittingBlock;
			}
			else if (ABlockBreaking* Block = Cast<ABlockBreaking>(Result.Actor))
			{
				if (HittingChunk->ContinueHit(Block, (*CurrentItem)->ItemS))
				{
					HittingBlock = nullptr;
					HittingChunk = nullptr;
				}
			}
		}
	}
}

void ULineTraceingInteractions::LeftClickEnd()
{
	bIsHitting = false;
	if (!HittingBlock)
		return;
	HittingChunk->CancelBreaking(HittingBlock);
	HittingBlock = nullptr;
	HittingChunk = nullptr;
}

void ULineTraceingInteractions::RightClickStart()
{
	if (!RayShootingCamera)
		return;
	FVector Start(RayShootingCamera->GetComponentLocation());
	FVector Direction = RayShootingCamera->GetComponentTransform().GetRotation().GetForwardVector();
	FHitResult Result;
	FVector EndPosition(Start + Direction * HandReachDistance);
	if (GetWorld()->LineTraceSingleByChannel(Result, Start, EndPosition, ECC_Visibility))
	{
		Item::PostUseTask Task = (*CurrentItem)->ItemS->OnItemUse(Result, World);
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

void ULineTraceingInteractions::RightClickEnd()
{
}

void ULineTraceingInteractions::SetCamera(UCameraComponent* Cam)
{
	RayShootingCamera = Cam;
}

void ULineTraceingInteractions::SetSelectedItemPointer(FItemStack** NewPointer)
{
	CurrentItem = NewPointer;
}
