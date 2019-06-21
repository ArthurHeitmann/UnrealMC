// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunk.h"
#include "Components/BoxComponent.h"
#include "Blocks/B_Stone.h"
#include "ProceduralMeshComponent/Public/ProceduralMeshComponent.h"
#include "PerlinNoise.h"
#include "McWorld.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"
#include "../Blocks/BlockBreaking.h"
#include "../Items/Item.h"
#include "ChunkSaveGame.h"
#include "../Misc/FileIO.h"

AChunk::AChunk()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	ChunkEnterTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Chunk Enter Trigger Box"));
	ChunkEnterTriggerBox->SetBoxExtent(FVector(799, 799, 12800));
	ChunkEnterTriggerBox->AddLocalOffset({800, 800, 12800});
	ChunkEnterTriggerBox->SetupAttachment(GetRootComponent());
	ChunkEnterTriggerBox->bHiddenInGame = false;	
	ChunkMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Chunk Procedural Mesh"));
	ChunkMesh->SetupAttachment(GetRootComponent());
}

void AChunk::BeginPlay()
{
	Super::BeginPlay();
	
	FVector TmpLoc = GetActorLocation();
	PosX = TmpLoc.X / 100;
	PosY = TmpLoc.Y / 100;
	ChunkEnterTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AChunk::ChunkEntered);
	
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), TSubclassOf<AMcWorld>(AMcWorld::StaticClass()), FoundActors);
	if (FoundActors.Num())
		McFWorld = Cast<AMcWorld>(FoundActors[0]);
	else
		UE_LOG(LogTemp, Warning, TEXT("Shit's about to go real bad..."))
	//SetMeshLifeStage(1);
}

void AChunk::EndPlay(EEndPlayReason::Type Reason)
{
	if (NorthChunk)
		NorthChunk->SetSouthChunk(nullptr);
	if (SouthChunk)
		SouthChunk->SetNorthChunk(nullptr);
	if (EastChunk)
		EastChunk->SetWestChunk(nullptr);
	if (WestChunk)
		WestChunk->SetEastChunk(nullptr);
	for (int x = 0; x < ChunkBlockData.Num(); x++)
	{
		for (int y = 0; y < ChunkBlockData[x].Num(); y++)
		{
			for (int z = 0; z < ChunkBlockData[x][y].Num(); z++)
			{
				if (ChunkBlockData[x][y][z] && ChunkBlockData[x][y][z]->GetBlockEnum() != Air)
					delete ChunkBlockData[x][y][z];
			}
		}
	}
	/*UChunkSaveGame* ChunkSave = Cast<UChunkSaveGame>(UGameplayStatics::CreateSaveGameObject(UChunkSaveGame::StaticClass()));
	ChunkSave->PosX = PosX / 16;
	ChunkSave->PosY = PosY / 16;
	ChunkSave->ParseData(ChunkBlockData);
	FString SaveName("Chunk_X_");
	SaveName += FString::FromInt(PosX / 16);
	SaveName += "_Y_";
	SaveName += FString::FromInt(PosY / 16);*/
	//UGameplayStatics::SaveGameToSlot(ChunkSave, SaveName, 0);
}

bool AChunk::ShouldFaceBePlacedBetween(Block* b1, Block* b2, TEnumAsByte<EFaceDirection> Side)
{
	return b2->IsSideOptimizable(Side)
			&& b2->GetBlockModelType() != BLOCK
		|| !b2->IsBlockOpaque();
}

void AChunk::ChunkEntered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Warning, TEXT("Chunk Entred by %s"), *FString(OtherActor->GetName()));
	
}

void AChunk::SetData(const TArray<TArray<TArray<Block*>>>& NewData, bool bUpdateMesh)
{
	ChunkBlockData = NewData;
	if (bUpdateMesh)
		bHasDataChanged = true;
}

void AChunk::SetHasDataChanged(bool state)
{
	bHasDataChanged = state;
}

void AChunk::UpdateMesh()
{
	if (!bHasDataChanged || !ChunkBlockData.Num())
		return;

	TMap<TEnumAsByte<EAllBlocks>, TArray<FVector>> Vertecies;
	TMap<TEnumAsByte<EAllBlocks>, TArray<FVector2D>> UVs;
	TMap<TEnumAsByte<EAllBlocks>, TArray<int32>> Triangles;
	TMap<TEnumAsByte<EAllBlocks>, TArray<FVector>> Normals;
	TMap<TEnumAsByte<EAllBlocks>, Block*> Materials;
	for (int x = 0; x < 16; x++)
	{
		for (int y = 0; y < 16; y++)
		{
			for (int z = 0; z < 99; z++)
			{
				if (ChunkBlockData[x][y][z]->GetBlockModelType() != TEnumAsByte<EBlockModelType>(EBlockModelType::NONE))
				{
					TEnumAsByte<EAllBlocks> cbe = ChunkBlockData[x][y][z]->GetBlockEnum(); //current block enum (of this iteration)
					if (!Materials.Contains(cbe))
					{
						Materials.Add(cbe, ChunkBlockData[x][y][z]);
						Vertecies.Add(cbe);
						UVs.Add(cbe);
						Triangles.Add(cbe);
						Normals.Add(cbe);
					}
					if (ChunkBlockData[x][y][z]->UsesCustomModel()) {
						Triangles[cbe].Append(ChunkBlockData[x][y][z]->GetAllTrainglesFrom(Vertecies[cbe].Num()));
						Vertecies[cbe].Append(ChunkBlockData[x][y][z]->GetAllVertecies(x * 100, y * 100, z * 100));
						UVs[cbe].Append(ChunkBlockData[x][y][z]->GetAllUVs());
						Normals[cbe].Append(ChunkBlockData[x][y][z]->GetAllNormals());
						continue;
					}
					if (z == 255 || ShouldFaceBePlacedBetween(ChunkBlockData[x][y][z], ChunkBlockData[x][y][z + 1], TOP))
					{
						Triangles[cbe].Append(ChunkBlockData[x][y][z]->GetTopTrianglesFrom(Vertecies[cbe].Num()));
						Vertecies[cbe].Append(ChunkBlockData[x][y][z]->GetTopVertecies(x * 100, y * 100, z * 100));
						UVs[cbe].Append(ChunkBlockData[x][y][z]->GetTopUVs());
						Normals[cbe].Append(ChunkBlockData[x][y][z]->GetTopNormals());
					}
					if (z == 0 || ShouldFaceBePlacedBetween(ChunkBlockData[x][y][z], ChunkBlockData[x][y][z - 1], BOTTOM))
					{
						Triangles[cbe].Append(ChunkBlockData[x][y][z]->GetBottomTrianglesFrom(Vertecies[cbe].Num()));
						Vertecies[cbe].Append(ChunkBlockData[x][y][z]->GetBottomVertecies(x * 100, y * 100, z * 100));
						UVs[cbe].Append(ChunkBlockData[x][y][z]->GetBottomUVs());
						Normals[cbe].Append(ChunkBlockData[x][y][z]->GetBottomNormals());
					}
					if (y != 15 && ShouldFaceBePlacedBetween(ChunkBlockData[x][y][z], ChunkBlockData[x][y + 1][z], RIGHT)
						|| 
						y == 15 && EastChunk &&  ShouldFaceBePlacedBetween(ChunkBlockData[x][y][z], EastChunk->GetBlockAt(x, 0, z), RIGHT))
					{
						Triangles[cbe].Append(ChunkBlockData[x][y][z]->GetRightTrianglesFrom(Vertecies[cbe].Num()));
						Vertecies[cbe].Append(ChunkBlockData[x][y][z]->GetRightVertecies(x * 100, y * 100, z * 100));
						UVs[cbe].Append(ChunkBlockData[x][y][z]->GetRightUVs());
						Normals[cbe].Append(ChunkBlockData[x][y][z]->GetRightNormals());
					}
					if (y && ShouldFaceBePlacedBetween(ChunkBlockData[x][y][z], ChunkBlockData[x][y - 1][z], LEFT)
						||
						y == 0 && WestChunk && ShouldFaceBePlacedBetween(ChunkBlockData[x][y][z], WestChunk->GetBlockAt(x, 15, z), LEFT))
					{
						Triangles[cbe].Append(ChunkBlockData[x][y][z]->GetLeftTrianglesFrom(Vertecies[cbe].Num()));
						Vertecies[cbe].Append(ChunkBlockData[x][y][z]->GetLeftVertecies(x * 100, y * 100, z * 100));
						UVs[cbe].Append(ChunkBlockData[x][y][z]->GetLeftUVs());
						Normals[cbe].Append(ChunkBlockData[x][y][z]->GetLeftNormals());
					}
					if (x && ShouldFaceBePlacedBetween(ChunkBlockData[x][y][z], ChunkBlockData[x - 1][y][z], SOUTH)
						||
						x == 0 && SouthChunk && ShouldFaceBePlacedBetween(ChunkBlockData[x][y][z], SouthChunk->GetBlockAt(15, y, z), SOUTH))
					{
						Triangles[cbe].Append(ChunkBlockData[x][y][z]->GetFrontTrianglesFrom(Vertecies[cbe].Num()));
						Vertecies[cbe].Append(ChunkBlockData[x][y][z]->GetFrontVertecies(x * 100, y * 100, z * 100));
						UVs[cbe].Append(ChunkBlockData[x][y][z]->GetFrontUVs());
						Normals[cbe].Append(ChunkBlockData[x][y][z]->GetFrontNormals());
					}
					if (x != 15 && ShouldFaceBePlacedBetween(ChunkBlockData[x][y][z], ChunkBlockData[x + 1][y][z], NORTH)
						||
						x == 15 && NorthChunk && ShouldFaceBePlacedBetween(ChunkBlockData[x][y][z], NorthChunk->GetBlockAt(0, y, z), NORTH))
					{
						Triangles[cbe].Append(ChunkBlockData[x][y][z]->GetBackTrianglesFrom(Vertecies[cbe].Num()));
						Vertecies[cbe].Append(ChunkBlockData[x][y][z]->GetBackVertecies(x * 100, y * 100, z * 100));
						UVs[cbe].Append(ChunkBlockData[x][y][z]->GetBackUVs());
						Normals[cbe].Append(ChunkBlockData[x][y][z]->GetBackNormals());
					}
				}
			}
		}
	}
	//for (const TPair<TEnumAsByte<EAllBlocks>, TArray<FVector>>& MeshCollection : Vertecies)
	for (auto i = Vertecies.CreateConstIterator(); i; ++i)
	{ 
		TEnumAsByte<EAllBlocks> key = i.Key();
		ChunkMesh->CreateMeshSection(key, i.Value(), Triangles[key], Normals[key], UVs[key], TArray<FColor>(), TArray<FProcMeshTangent>(), true);
		ChunkMesh->SetMaterial(key, Materials[key]->GetMaterial(this));
	}


	bHasDataChanged = false;
}

void AChunk::SetMeshLifeStage(int Stage)
{
	if (LifeStage == 3)
		Destroy();
	switch (Stage)
	{
	case 0:
		if (LifeStage != 0)
		{
			ChunkMesh->SetVisibility(true);
			ChunkMesh->SetRenderInMainPass(true);
		}
		break;
	case 1:
		if (LifeStage != 1)
		{
			ChunkMesh->SetVisibility(true);
			ChunkMesh->SetRenderInMainPass(false);
		}
		break;
	case 2:
		if (LifeStage != 2)
		{
			ChunkMesh->SetVisibility(false);
		}
		break;
	case 3:
		Destroy();
	}
	LifeStage = Stage;
}

Block* AChunk::RegisterHitAt(const FHitResult& HitResult, Item* Item)
{
	//what a mess
	Block* HitBlock;
	FVector HitLocation(((int) HitResult.ImpactPoint.X / 100) % 16, ((int) HitResult.ImpactPoint.Y / 100) % 16, ((int) HitResult.ImpactPoint.Z / 100) % 256);
	
	bool SkipXNegativeAdj = false;
	bool SkipYNegativeAdj = false;
	if (HitLocation.X == 0 && ((int) HitResult.ImpactPoint.X / 100) != PosX)
	{
		HitLocation.X = 15;
		SkipXNegativeAdj = true;
	}
	if (HitLocation.Y == 0 && ((int) HitResult.ImpactPoint.Y / 100) != PosY)
	{
		HitLocation.Y = 15;
		SkipYNegativeAdj = true;
	}


	int xChange = -1;
	int yChange = -1;
 	if (PosX < 0)
	{
		if (!SkipXNegativeAdj && floorf(HitResult.ImpactPoint.X) != HitResult.ImpactPoint.X)
			HitLocation.X += 15;	
		xChange = 1;
	}
	if (PosY < 0)
	{
		if (!SkipYNegativeAdj && floorf(HitResult.ImpactPoint.Y) != HitResult.ImpactPoint.Y)
			HitLocation.Y += 15;
		yChange = 1;
	}
	if (HitLocation.X < 0)
		HitLocation.X += 15;
	if (HitLocation.Y < 0)
		HitLocation.Y += 15;

	if (ChunkBlockData[HitLocation.X][HitLocation.Y][HitLocation.Z]->GetBlockEnum() == Air)
	{
		if (floorf(HitResult.ImpactPoint.X) == HitResult.ImpactPoint.X)
		{
			if (xChange == 1 && HitLocation.X < 15 || xChange == -1 && HitLocation.X > 0)
				HitLocation.X += xChange;
		}
		else if (HitLocation.Y && HitLocation.Y < 15 && floorf(HitResult.ImpactPoint.Y) == HitResult.ImpactPoint.Y)
		{
			if (yChange == 1 && HitLocation.Y < 15 || yChange == -1 && HitLocation.Y > 0)
				HitLocation.Y += yChange;
		}
		else if (HitLocation.Z && floorf(HitResult.ImpactPoint.Z) == HitResult.ImpactPoint.Z)
			HitLocation.Z += -1;
	}

	HitBlock = ChunkBlockData[HitLocation.X][HitLocation.Y][HitLocation.Z];

	if (HitBlock->GetBlockEnum() == Air)
		return nullptr;
	
	float BreakingTime = GetWorld()->GetDeltaSeconds() * Item->GetBreakingSpeed();
	HitBlock->OnBeginBreak(GetWorld(), (HitLocation + FVector(PosX, PosY, 0)) * 100);
	//TODO delete old ref
	ChunkBlockData[HitLocation.X][HitLocation.Y][HitLocation.Z] = McFWorld->GetBlock(Air);
	bHasDataChanged = true;
	ABlockBreaking* BB = GetWorld()->SpawnActor<ABlockBreaking>(HitLocation * 100 + FVector(PosX * 100, PosY * 100, 0), FRotator::ZeroRotator);;
	BB->InitWithBlock(HitBlock);
	BB->SetBreakingStage(BreakingTime / HitBlock->GetBreakTime());
	BreakingBlocks.Add({ BB, 0.f, Item, (int) HitLocation.X, (int) HitLocation.Y, (int) HitLocation.Z });

	if (HitLocation.X == 0 && SouthChunk)
		SouthChunk->SetHasDataChanged();
	if (HitLocation.X == 15 && NorthChunk)
		NorthChunk->SetHasDataChanged();
	if (HitLocation.Y == 0 && WestChunk)
		WestChunk->SetHasDataChanged();
	if (HitLocation.Y == 15 && EastChunk)
		EastChunk->SetHasDataChanged();

	return HitBlock;
}

bool AChunk::ContinueHit(ABlockBreaking* Block, class Item* Item)
{
	for (BlockBreakingData& BBData : BreakingBlocks)
	{
		if (BBData.Block == Block)
		{
			if (Item != BBData.ItemUsed)
			{
				BBData.HitTime = GetWorld()->GetDeltaSeconds() * Item->GetBreakingSpeed();
				BBData.Block->GetBlock()->OnBeginBreak(GetWorld(), Block->GetActorLocation());
			}
			else
				BBData.HitTime += GetWorld()->GetDeltaSeconds() * Item->GetBreakingSpeed();
			float BreakingPercentage = BBData.HitTime / Block->GetBlock()->GetBreakTime();
			if (BreakingPercentage >= 1)
			{
				Block->GetBlock()->OnBreak(GetWorld(), Block->GetActorLocation());
				Block->Destroy();
				return true;
			}
			BBData.Block->SetBreakingStage(BreakingPercentage);
			return false;
		}
	}
	return false;
}

void AChunk::CancelBreaking(Block* Block)
{
	for (int i = 0; i < BreakingBlocks.Num(); i++)
	{
		if (BreakingBlocks[i].Block->GetBlock() == Block)
		{
			ChunkBlockData[BreakingBlocks[i].x][BreakingBlocks[i].y][BreakingBlocks[i].z] = Block;
			bHasDataChanged = true;
			UpdateMesh();
			BreakingBlocks[i].Block->Destroy();
			FIntPoint HitLocation(BreakingBlocks[i].x, BreakingBlocks[i].y);
			if (HitLocation.X == 0 && SouthChunk)
				SouthChunk->SetHasDataChanged();
			if (HitLocation.X == 15 && NorthChunk)
				NorthChunk->SetHasDataChanged();
			if (HitLocation.Y == 0 && WestChunk)
				WestChunk->SetHasDataChanged();
			if (HitLocation.Y == 15 && EastChunk)
				EastChunk->SetHasDataChanged();
			BreakingBlocks.RemoveAtSwap(i);
			return;
		}
	}
}

int AChunk::GetNextGenerationStage()
{
	return NextGenerationStage;
}

void AChunk::SetNextGenerationStage(int NewStage)
{
	NextGenerationStage = NewStage;
}

TArray<TArray<TArray<Block*>>>* AChunk::GetChunkBlockData()
{
	return &ChunkBlockData;
}

Block* AChunk::GetBlockAt(int x, int y, int z)
{
	return ChunkBlockData[x][y][z];
}

void AChunk::SetNorthChunk(AChunk* c)
{
	NorthChunk = c;
	bHasDataChanged = true;
}

void AChunk::SetEastChunk(AChunk* c)
{
	EastChunk = c;
	bHasDataChanged = true;
}

void AChunk::SetSouthChunk(AChunk* c)
{
	SouthChunk = c;
	bHasDataChanged = true;
}

void AChunk::SetWestChunk(AChunk* c)
{
	WestChunk = c;
	bHasDataChanged = true;
}

uint64 AChunk::GetLastTimeUpdated()
{
	return LastTimeUpdated;
}

void AChunk::SetLastTimeUpdated(float NewTime)
{
	LastTimeUpdated = NewTime;
	//TODO update ticks
}

void AChunk::Tick(float Delta)
{
	if (bHasDataChanged)
		UpdateMesh();

	LastTimeUpdated += Delta;
}
