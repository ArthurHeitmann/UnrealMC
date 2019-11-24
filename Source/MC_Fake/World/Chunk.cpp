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
#include "ChunkFormCoords.h"

Chunk::Chunk(ChunkFormCoords2D Pos, class AMcWorld* McWorld)
{
	this->Pos = Pos;
	this->McWorld = McWorld;

	Root = NewObject<USceneComponent>(McWorld);
	Root->SetupAttachment(McWorld->ChunksRoot);
	Root->RegisterComponent();
	Root->AddWorldOffset({ Pos.x * 1600.f, Pos.y * 1600.f, 0.f });
	CubesRoot = NewObject<USceneComponent>(McWorld);
	CubesRoot->SetupAttachment(Root);
	CubesRoot->RegisterComponent();
	CubesRoot->AddLocalOffset({ 0.f, 0.f, 300.f });
	ChunkEnterTriggerBox = NewObject<UBoxComponent>(McWorld);
	ChunkEnterTriggerBox->SetupAttachment(Root);
	ChunkEnterTriggerBox->RegisterComponent();
	ChunkEnterTriggerBox->SetBoxExtent(FVector(799, 799, 50));
	ChunkEnterTriggerBox->AddLocalOffset({800, 800, 50});
	//ChunkEnterTriggerBox->bHiddenInGame = false;	
	//ChunkEnterTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &Chunk::ChunkEntered);
}

ChunkFormCoords2D Chunk::GetPos()
{
	return Pos;
}

Chunk::~Chunk()
{
	McWorld->RemoveLoadedChunk(Pos);

	TArray<USceneComponent*> comps;
	Root->GetChildrenComponents(true, comps);
	for (auto& comp : comps)
	{
		comp->DestroyComponent();
	}

	Root->DestroyComponent();

	for (auto& cube : ChunkCubes)
	{
		delete cube.Value;
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

void Chunk::CreateChunkCube(int8 PosZ)
{
	ChunkCube* NewCube = new ChunkCube({ Pos.x, Pos.y, PosZ }, McWorld, this);
	//NewCube->SetupAttachment(CubesRoot);
	//NewCube->AttachToComponent(CubesRoot, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false));
	ChunkCubes.Add(PosZ, NewCube);
	McWorld->AddLoadedChunkCube(NewCube, NewCube->GetPos());
	McWorld->AddChunkGenTask(NewCube);
	//NewCube->SetWorldLocation(GetActorLocation() + FVector{ Pos.x * 1600.f, Pos.y * 1600.f, PosZ * 1600.f });
}

void Chunk::DequeueChunkCubes()
{
	while (!ChunkCubesCreatingBuffer.IsEmpty())
	{
		int8 PosZ;
		ChunkCubesCreatingBuffer.Dequeue(PosZ);
		CreateChunkCube(PosZ);
	}
}

//bool Chunk::ShouldFaceBePlacedBetween(Block* b1, Block* b2, TEnumAsByte<EDirection> Side)
//{
//	return b2->IsSideOptimizable(Side)
//			&& b2->GetBlockModelType() != BLOCK
//		|| !b2->IsBlockOpaque();
//}

void Chunk::ChunkEntered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Warning, TEXT("Chunk Entred by %s"), *FString(OtherActor->GetName()));
	
}

//void Chunk::SetHasDataChanged(bool state)
//{
//	bHasDataChanged = state;
//}

void Chunk::SetHasFinishedGenerating(bool state = true)
{
	bHasFinishedGenerating = state;
}

bool Chunk::GetHasFinishedGenerating()
{
	return bHasFinishedGenerating;
}

//void Chunk::UpdateMesh()
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

void Chunk::SetMeshLifeStage(int Stage)
{
	if (LifeStage == 3)
	{
		delete this;
		return;
	}
	for (auto& Cube : ChunkCubes)
	{
		switch (Stage)
		{
		case 0:
			if (LifeStage != 0)//TODO CR
			{
				//Cube.Value->SetVisibility(true);
				//Cube.Value->SetRenderInMainPass(true);
			}
			break;
		case 1:
			if (LifeStage != 1)
			{
				//Cube.Value->SetVisibility(true);
				//Cube.Value->SetRenderInMainPass(false);
			}
			break;
		case 2:
			if (LifeStage != 2)
			{
				//Cube.Value->SetVisibility(false);
			}
			break;
		case 3:
			delete this;
			return;
		}
	}
	LifeStage = Stage;
}

void Chunk::UpdateChunkCubesLoading(int8 BaseHeight, int8 RangeDown, int8 RangeUp)
{
	for (int8 z = -RangeDown; z <= RangeUp; z++)
	{
		int8 PosZ = BaseHeight + z;
		if (PosZ >= 0 && PosZ < 16 && !ChunkCubes.Contains(PosZ))
			//ChunkCubesCreatingBuffer.Enqueue(PosZ);
			CreateChunkCube(PosZ);
	}
}

Block* Chunk::RegisterHitAt(const FHitResult& HitResult, Item* Item)
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

bool Chunk::ContinueHit(ABlockBreaking* Block, class Item* Item)
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

void Chunk::CancelBreaking(Block* Block)
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

TMap<int8, class ChunkCube*>& Chunk::GetChunkCubes()
{
	return ChunkCubes;
}

Block* Chunk::GetBlockAt(int x, int y, int z)
{
	int8 Key = z / 16;
	if (ChunkCubes.Contains(Key))
		return (*ChunkCubes.Find(Key))->GetBlockAt(x, y, Key);
	else
		return nullptr;
}

Block*& Chunk::GetBlockAtAsRef(int x, int y, int z)
{
	int8 Key = z / 16;
	return (*ChunkCubes.Find(Key))->GetBlockAt(x, y, Key);
}

ChunkGenMaps& Chunk::GetChunkGenMaps()
{
	return ChunkDataMaps;
}

//void Chunk::SetNorthChunk(Chunk* c)
//{
//	NorthChunk = c;
//	bHasDataChanged = true;
//}
//
//void Chunk::SetEastChunk(Chunk* c)
//{
//	EastChunk = c;
//	bHasDataChanged = true;
//}
//
//void Chunk::SetSouthChunk(Chunk* c)
//{
//	SouthChunk = c;
//	bHasDataChanged = true;
//}
//
//void Chunk::SetWestChunk(Chunk* c)
//{
//	WestChunk = c;
//	bHasDataChanged = true;
//}

void Chunk::ToggleChunkBorders()
{
	ChunkEnterTriggerBox->SetVisibility(!ChunkEnterTriggerBox->IsVisible());
}

//uint64 Chunk::GetLastTimeUpdated()
//{
//	return LastTimeUpdated;
//}

//void Chunk::SetLastTimeUpdated(float NewTime)
//{
//	LastTimeUpdated = NewTime;
//}

void Chunk::Tick(float Delta)
{
	for (auto& cube : ChunkCubes)
	{
		cube.Value->Tick(Delta);
	}
	
	//DequeueChunkCubes();

}
