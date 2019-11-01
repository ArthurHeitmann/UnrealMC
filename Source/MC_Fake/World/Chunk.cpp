// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunk.h"
#include "Components/BoxComponent.h"
#include "Blocks/B_Stone.h"
#include "RuntimeMeshComponent.h"
#include "PerlinNoise.h"
#include "McWorld.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"
#include "../Blocks/BlockBreaking.h"
#include "../Items/Item.h"
#include "ChunkSaveGame.h"
#include "../Misc/FileIO.h"
#include "GameFramework/Character.h"
#include "ChunkCube.h"

AChunk::AChunk()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	ChunkEnterTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Chunk Enter Trigger Box"));
	ChunkEnterTriggerBox->SetBoxExtent(FVector(799, 799, 12800));
	ChunkEnterTriggerBox->AddLocalOffset({800, 800, 12800});
	ChunkEnterTriggerBox->SetupAttachment(GetRootComponent());
	//ChunkEnterTriggerBox->bHiddenInGame = false;	
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

void AChunk::LoadChunkCube(int8 Pos)
{
	UChunkCube* NewCube = NewObject<UChunkCube>(this);

	NewCube->RegisterComponent();
	NewCube->AttachToComponent(GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false));
	ChunkCubes.Add(Pos, NewCube);
	//NewCube->SetWorldLocation({ PosX * 1600.f, PosY * 1600.f, 0.f });
}

//bool AChunk::ShouldFaceBePlacedBetween(Block* b1, Block* b2, TEnumAsByte<EDirection> Side)
//{
//	return b2->IsSideOptimizable(Side)
//			&& b2->GetBlockModelType() != BLOCK
//		|| !b2->IsBlockOpaque();
//}

void AChunk::ChunkEntered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Warning, TEXT("Chunk Entred by %s"), *FString(OtherActor->GetName()));
	
}

//void AChunk::SetHasDataChanged(bool state)
//{
//	bHasDataChanged = state;
//}

void AChunk::SetHasFinishedGenerating(bool state = true)
{
	bHasFinishedGenerating = state;
}

//void AChunk::UpdateMesh()
//{
//	if (!bHasDataChanged || !ChunkBlockData.Num() || !bHasFinishedGenerating)
//		return;
//	TMap<TEnumAsByte<EAllBlocks>, TArray<FVector>> Vertecies;
//	TMap<TEnumAsByte<EAllBlocks>, TArray<FVector2D>> UVs;
//	TMap<TEnumAsByte<EAllBlocks>, TArray<int32>> Triangles;
//	TMap<TEnumAsByte<EAllBlocks>, TArray<FVector>> Normals;
//	TMap<TEnumAsByte<EAllBlocks>, Block*> Materials;
//	for (int x = 0; x < 16; x++)
//	{
//		for (int y = 0; y < 16; y++)
//		{
//			for (int z = 0; z < 256; z++)
//			{
//				if (ChunkBlockData[x][y][z]->GetBlockModelType() != TEnumAsByte<EBlockModelType>(EBlockModelType::NONE))
//				{
//					TEnumAsByte<EAllBlocks> cbe = ChunkBlockData[x][y][z]->GetBlockEnum(); //current block enum (of this iteration)
//					if (!Materials.Contains(cbe))
//					{
//						Materials.Add(cbe, ChunkBlockData[x][y][z]);
//						Vertecies.Add(cbe);
//						UVs.Add(cbe);
//						Triangles.Add(cbe);
//						Normals.Add(cbe);
//					}
//					if (ChunkBlockData[x][y][z]->UsesCustomModel()) {
//						Triangles[cbe].Append(ChunkBlockData[x][y][z]->GetAllTrainglesFrom(Vertecies[cbe].Num()));
//						Vertecies[cbe].Append(ChunkBlockData[x][y][z]->GetAllVertecies(x * 100, y * 100, z * 100));
//						UVs[cbe].Append(ChunkBlockData[x][y][z]->GetAllUVs());
//						Normals[cbe].Append(ChunkBlockData[x][y][z]->GetAllNormals());
//						continue;
//					}
//					if (z == 255 || ShouldFaceBePlacedBetween(ChunkBlockData[x][y][z], ChunkBlockData[x][y][z + 1], TOP))
//					{
//						Triangles[cbe].Append(ChunkBlockData[x][y][z]->GetTopTrianglesFrom(Vertecies[cbe].Num()));
//						Vertecies[cbe].Append(ChunkBlockData[x][y][z]->GetTopVertecies(x * 100, y * 100, z * 100));
//						UVs[cbe].Append(ChunkBlockData[x][y][z]->GetTopUVs());
//						Normals[cbe].Append(ChunkBlockData[x][y][z]->GetTopNormals());
//					}
//					if (z == 0 || ShouldFaceBePlacedBetween(ChunkBlockData[x][y][z], ChunkBlockData[x][y][z - 1], BOTTOM))
//					{
//						Triangles[cbe].Append(ChunkBlockData[x][y][z]->GetBottomTrianglesFrom(Vertecies[cbe].Num()));
//						Vertecies[cbe].Append(ChunkBlockData[x][y][z]->GetBottomVertecies(x * 100, y * 100, z * 100));
//						UVs[cbe].Append(ChunkBlockData[x][y][z]->GetBottomUVs());
//						Normals[cbe].Append(ChunkBlockData[x][y][z]->GetBottomNormals());
//					}
//					if (y != 15 && ShouldFaceBePlacedBetween(ChunkBlockData[x][y][z], ChunkBlockData[x][y + 1][z], EAST)
//						|| 
//						y == 15 && EastChunk &&  ShouldFaceBePlacedBetween(ChunkBlockData[x][y][z], EastChunk->GetBlockAt(x, 0, z), EAST))
//					{
//						Triangles[cbe].Append(ChunkBlockData[x][y][z]->GetRightTrianglesFrom(Vertecies[cbe].Num()));
//						Vertecies[cbe].Append(ChunkBlockData[x][y][z]->GetRightVertecies(x * 100, y * 100, z * 100));
//						UVs[cbe].Append(ChunkBlockData[x][y][z]->GetRightUVs());
//						Normals[cbe].Append(ChunkBlockData[x][y][z]->GetRightNormals());
//					}
//					if (y && ShouldFaceBePlacedBetween(ChunkBlockData[x][y][z], ChunkBlockData[x][y - 1][z], WEST)
//						||
//						y == 0 && WestChunk && ShouldFaceBePlacedBetween(ChunkBlockData[x][y][z], WestChunk->GetBlockAt(x, 15, z), WEST))
//					{
//						Triangles[cbe].Append(ChunkBlockData[x][y][z]->GetLeftTrianglesFrom(Vertecies[cbe].Num()));
//						Vertecies[cbe].Append(ChunkBlockData[x][y][z]->GetLeftVertecies(x * 100, y * 100, z * 100));
//						UVs[cbe].Append(ChunkBlockData[x][y][z]->GetLeftUVs());
//						Normals[cbe].Append(ChunkBlockData[x][y][z]->GetLeftNormals());
//					}
//					if (x && ShouldFaceBePlacedBetween(ChunkBlockData[x][y][z], ChunkBlockData[x - 1][y][z], SOUTH)
//						||
//						x == 0 && SouthChunk && ShouldFaceBePlacedBetween(ChunkBlockData[x][y][z], SouthChunk->GetBlockAt(15, y, z), SOUTH))
//					{
//						Triangles[cbe].Append(ChunkBlockData[x][y][z]->GetFrontTrianglesFrom(Vertecies[cbe].Num()));
//						Vertecies[cbe].Append(ChunkBlockData[x][y][z]->GetFrontVertecies(x * 100, y * 100, z * 100));
//						UVs[cbe].Append(ChunkBlockData[x][y][z]->GetFrontUVs());
//						Normals[cbe].Append(ChunkBlockData[x][y][z]->GetFrontNormals());
//					}
//					if (x != 15 && ShouldFaceBePlacedBetween(ChunkBlockData[x][y][z], ChunkBlockData[x + 1][y][z], NORTH)
//						||
//						x == 15 && NorthChunk && ShouldFaceBePlacedBetween(ChunkBlockData[x][y][z], NorthChunk->GetBlockAt(0, y, z), NORTH))
//					{
//						Triangles[cbe].Append(ChunkBlockData[x][y][z]->GetBackTrianglesFrom(Vertecies[cbe].Num()));
//						Vertecies[cbe].Append(ChunkBlockData[x][y][z]->GetBackVertecies(x * 100, y * 100, z * 100));
//						UVs[cbe].Append(ChunkBlockData[x][y][z]->GetBackUVs());
//						Normals[cbe].Append(ChunkBlockData[x][y][z]->GetBackNormals());
//					}
//				}
//			}
//		}
//	}
//	//for (const TPair<TEnumAsByte<EAllBlocks>, TArray<FVector>>& MeshCollection : Vertecies)
//	for (auto i = Vertecies.CreateConstIterator(); i; ++i)
//	{ 
//		TEnumAsByte<EAllBlocks> key = i.Key();
//		ChunkMesh->CreateMeshSection(key, i.Value(), Triangles[key], Normals[key], UVs[key], TArray<FColor>(), TArray<FRuntimeMeshTangent>(), true);
//		ChunkMesh->SetMaterial(key, Materials[key]->GetMaterial(this));
//	}
//
//
//	bHasDataChanged = false;
//}

void AChunk::SetMeshLifeStage(int Stage)
{
	if (LifeStage == 3)
		Destroy();
	for (auto& Cube : ChunkCubes)
	{
		switch (Stage)
		{
		case 0:
			if (LifeStage != 0)
			{
				Cube.Value->SetVisibility(true);
				Cube.Value->SetRenderInMainPass(true);
			}
			break;
		case 1:
			if (LifeStage != 1)
			{
				Cube.Value->SetVisibility(true);
				Cube.Value->SetRenderInMainPass(false);
			}
			break;
		case 2:
			if (LifeStage != 2)
			{
				Cube.Value->SetVisibility(false);
			}
			break;
		case 3:
			Destroy();
		}
	}
	LifeStage = Stage;
}

void AChunk::UpdateChunkCubesLoading(int8 BaseHeight, int8 RangeDown, int8 RangeUp)
{
	for (int8 z = -RangeDown; z <= RangeUp; z++)
	{
		int8 PosZ = BaseHeight + z;
		if (PosZ >= 0 && PosZ < 16 && !ChunkCubes.Contains(PosZ))
			LoadChunkCube(PosZ);
	}
}

Block* AChunk::RegisterHitAt(const FHitResult& HitResult, Item* Item)
{
	////what a mess
	//Block* HitBlock;
	//FVector HitLocation(((int) HitResult.Location.X / 100) % 16, ((int) HitResult.Location.Y / 100) % 16, ((int) HitResult.Location.Z / 100) % 256);
	//
	//bool SkipXNegativeAdj = false;
	//bool SkipYNegativeAdj = false;
	//if (HitLocation.X == 0 && ((int) HitResult.Location.X / 100) != PosX)
	//{
	//	HitLocation.X = 15;
	//	SkipXNegativeAdj = true;
	//}
	//if (HitLocation.Y == 0 && ((int) HitResult.Location.Y / 100) != PosY)
	//{
	//	HitLocation.Y = 15;
	//	SkipYNegativeAdj = true;
	//}


	//int xChange = -1;
	//int yChange = -1;
 //	if (PosX < 0)
	//{
	//	if (!SkipXNegativeAdj && floorf(HitResult.Location.X) != HitResult.Location.X)
	//		HitLocation.X += 15;	
	//	xChange = 1;
	//}
	//if (PosY < 0)
	//{
	//	if (!SkipYNegativeAdj && floorf(HitResult.Location.Y) != HitResult.Location.Y)
	//		HitLocation.Y += 15;
	//	yChange = 1;
	//}
	//if (HitLocation.X < 0)
	//	HitLocation.X += 15;
	//if (HitLocation.Y < 0)
	//	HitLocation.Y += 15;

	//if (!ChunkCubes.Contains((int8) HitLocation.Z / 16))
	//{
	//	UE_LOG(LogTemp, Error, TEXT("Hitting none existing Chunk Cube at x: %f y: %f z: %f"), HitLocation.X, HitLocation.Y, HitLocation.Z);
	//	throw "Chunk hit Exception";
	//}

	//if (GetBlockAtAsRef(HitLocation.X, HitLocation.Y, HitLocation.Z)->GetBlockEnum() == BAir)
	//{
	//	if (floorf(HitResult.Location.X) == HitResult.Location.X)
	//	{
	//		if (xChange == 1 && HitLocation.X < 15 || xChange == -1 && HitLocation.X > 0)
	//			HitLocation.X += xChange;
	//	}
	//	else if (HitLocation.Y && HitLocation.Y < 15 && floorf(HitResult.Location.Y) == HitResult.Location.Y)
	//	{
	//		if (yChange == 1 && HitLocation.Y < 15 || yChange == -1 && HitLocation.Y > 0)
	//			HitLocation.Y += yChange;
	//	}
	//	else if (HitLocation.Z && floorf(HitResult.Location.Z) == HitResult.Location.Z)
	//		HitLocation.Z += -1;
	//}

	//HitBlock = GetBlockAtAsRef(HitLocation.X, HitLocation.Y, HitLocation.Z);

	//if (HitBlock->GetBlockEnum() == BAir)
	//	return nullptr;
	//float multiplier = 1;
	//if (Item->GetItemActions().Contains(HitBlock->GetBreakingAction()))
	//	multiplier = Item->GetBreakingSpeed();
	//float BreakingTime = GetWorld()->GetDeltaSeconds() * multiplier;
	//HitBlock->OnBeginBreak(GetWorld(), (HitLocation + FVector(PosX, PosY, 0)) * 100);
	//GetBlockAtAsRef(HitLocation.X, HitLocation.Y, HitLocation.Z) = McFWorld->GetBlockFromEnum(BAir);
	//bHasDataChanged = true;
	//ABlockBreaking* BB = GetWorld()->SpawnActor<ABlockBreaking>(HitLocation * 100 + FVector(PosX * 100, PosY * 100, 0), FRotator::ZeroRotator);;
	//BB->InitWithBlock(HitBlock);
	//BB->SetBreakingStage(BreakingTime / HitBlock->GetBreakTime());
	//BreakingBlocks.Add({ BB, 0.f, Item, (int) HitLocation.X, (int) HitLocation.Y, (int) HitLocation.Z });

	//if (HitLocation.X == 0 && SouthChunk)
	//	SouthChunk->SetHasDataChanged();
	//if (HitLocation.X == 15 && NorthChunk)
	//	NorthChunk->SetHasDataChanged();
	//if (HitLocation.Y == 0 && WestChunk)
	//	WestChunk->SetHasDataChanged();
	//if (HitLocation.Y == 15 && EastChunk)
	//	EastChunk->SetHasDataChanged();

	//return HitBlock;
	return nullptr;
}

bool AChunk::ContinueHit(ABlockBreaking* Block, class Item* Item)
{
	/*for (BlockBreakingData& BBData : BreakingBlocks)
	{
		if (BBData.Block == Block)
		{
			float multiplier = 1;
			if (Item->GetItemActions().Contains(BBData.Block->GetBlock()->GetBreakingAction()))
				multiplier = Item->GetBreakingSpeed();
			if (Item != BBData.ItemUsed)
			{
				BBData.HitTime = GetWorld()->GetDeltaSeconds() * multiplier;
				BBData.Block->GetBlock()->OnBeginBreak(GetWorld(), Block->GetActorLocation());
			}
			else
				BBData.HitTime += GetWorld()->GetDeltaSeconds() * multiplier;
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
	}*/
	return false;
}

void AChunk::CancelBreaking(Block* Block)
{
	/*for (int i = 0; i < BreakingBlocks.Num(); i++)
	{
		if (BreakingBlocks[i].Block->GetBlock() == Block)
		{
			GetBlockAtAsRef(BreakingBlocks[i].x, BreakingBlocks[i].y, BreakingBlocks[i].z) = Block;
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
	}*/
}

TMap<int8, class UChunkCube*>& AChunk::GetChunkCubes()
{
	return ChunkCubes;
}

Block* AChunk::GetBlockAt(int x, int y, int z)
{
	int8 Key = z / 16;
	if (ChunkCubes.Contains(Key))
		return (*ChunkCubes.Find(Key))->GetBlockAt(x, y, Key);
	else
		return nullptr;
}

Block*& AChunk::GetBlockAtAsRef(int x, int y, int z)
{
	int8 Key = z / 16;
	return (*ChunkCubes.Find(Key))->GetBlockAt(x, y, Key);
}

ChunkGenMaps& AChunk::GetChunkGenMaps()
{
	return ChunkDataMaps;
}

//void AChunk::SetNorthChunk(AChunk* c)
//{
//	NorthChunk = c;
//	bHasDataChanged = true;
//}
//
//void AChunk::SetEastChunk(AChunk* c)
//{
//	EastChunk = c;
//	bHasDataChanged = true;
//}
//
//void AChunk::SetSouthChunk(AChunk* c)
//{
//	SouthChunk = c;
//	bHasDataChanged = true;
//}
//
//void AChunk::SetWestChunk(AChunk* c)
//{
//	WestChunk = c;
//	bHasDataChanged = true;
//}

void AChunk::ToggleChunkBorders()
{
	ChunkEnterTriggerBox->SetVisibility(!ChunkEnterTriggerBox->IsVisible());
}

//uint64 AChunk::GetLastTimeUpdated()
//{
//	return LastTimeUpdated;
//}

//void AChunk::SetLastTimeUpdated(float NewTime)
//{
//	LastTimeUpdated = NewTime;
//}

void AChunk::Tick(float Delta)
{
}
