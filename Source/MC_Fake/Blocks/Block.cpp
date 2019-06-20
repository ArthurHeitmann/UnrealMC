// Fill out your copyright notice in the Description page of Project Settings.


#include "Block.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"
#include "../Items/Item.h"
#include "../Items/ItemDrop.h"
#include "../Items/I_BlockItem.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "B_Air.h"
#include "B_Stone.h"
#include "B_Dirt.h"
#include "B_Grass.h"
#include "B_Water.h"

UMaterial* Block::BlockMaterial;

Block::Block()
{
	BlockID = -1;
	SubID = 0;
	BlockName = TEXT("No Name Block");
	BlockModelType = BLOCK;
	BreakTime = 0.75f;	
	if (!Block::BlockMaterial)
	{
		static ConstructorHelpers::FObjectFinder<UMaterial> MaterialInstanceObj(TEXT("Material'/Game/Materials/Blocks/M_BlockDefault.M_BlockDefault'"));
		Block::BlockMaterial = MaterialInstanceObj.Object;
	}
}

Block::~Block()
{
}

Block* Block::GetBlockFromBytes(uint8* Bytes)
{
	uint16* id = reinterpret_cast<uint16*>(Bytes);
	switch (*id)
	{
	case 0:
		return B_Air::AirRef;
	case 1:
		return new B_Stone();
	case 2:
		return new B_Dirt();
	case 3:
		return new B_Grass();
	case 9:
		return new B_Water();
	default:
		return nullptr;
	}
}

void Block::DropItem(UWorld* World, FVector Location, Item* DropItem)
{
	AItemDrop* Drop = World->SpawnActor<AItemDrop>(Location + FVector(50, 50, 50), FRotator::ZeroRotator);
	Drop->SetMesh(GetAllVertecies(-50, -50, -50), GetAllTrainglesFrom(0), GetAllUVs(), GetAllNormals(), GetMaterial(Drop));
}

bool Block::IsSideOptimizable(TEnumAsByte<EFaceDirection> Direction)
{
	return true;
}

bool Block::IsBlockOpaque()
{
	return true;
}

bool Block::UsesCustomModel()
{
	return false;
}

bool Block::HasConstantMaterial()
{
	return true;
}

float Block::GetBreakTime()
{
	return BreakTime;
}

Block Block::GetSelf()
{
	return *this;
}

TArray<uint8> Block::GetBinaryData()
{
	uint8* Bytes = reinterpret_cast<uint8*>(FMemory::Malloc(sizeof(uint8) * 3));
	Bytes[0] = BlockID & 255;
	Bytes[1] = BlockID & 65280;
	Bytes[2] = SubID & 127;
	TArray<uint8> out = {
		Bytes[0], Bytes[1], Bytes[2]
	};
	delete Bytes;
	return out;
}

void Block::OnPlace(class UWorld* World, FVector Location)
{
}

void Block::OnLeftclick(class UWorld* World, FVector Location)
{
}

void Block::OnBeginBreak(class UWorld* World, FVector Location)
{
}

void Block::Updatebreak(float Millis)
{
}

void Block::OnBreak(class UWorld* World, FVector Location)
{
	DropItem(World, Location, new I_BlockItem(this));
}

TEnumAsByte<EBlockModelType> Block::GetBlockModelType()
{
	return BlockModelType;
}

TEnumAsByte<EAllBlocks> Block::GetBlockEnum()
{
	return BlockEnum;
}

UMaterialInstanceDynamic* Block::GetMaterial(UObject* UObj)
{
	return UMaterialInstanceDynamic::Create(BlockMaterial, UObj);
}

TArray<FVector> Block::GetTopVertecies(float x, float y, float z)
{
	return {
		{x, y, z + 100},
		{x, y + 100, z + 100},
		{x + 100, y + 100, z + 100},
		{x + 100, y, z + 100},
	};
}

TArray<FVector> Block::GetRightVertecies(float x, float y, float z)
{
	return {
		{x, y + 100, z},
		{x + 100, y + 100, z},
		{x + 100, y + 100, z + 100},
		{x, y + 100, z + 100},
	};
}

TArray<FVector> Block::GetBottomVertecies(float x, float y, float z)
{
	return {
		{x, y, z},
		{x + 100, y, z},
		{x + 100, y + 100, z},
		{x, y + 100, z},
	};
}

TArray<FVector> Block::GetLeftVertecies(float x, float y, float z)
{
	return {
		{x, y, z},
		{x, y, z + 100},
		{x + 100, y, z + 100},
		{x + 100, y, z},
	};
}

TArray<FVector> Block::GetFrontVertecies(float x, float y, float z)
{
	return {
		{x, y, z},
		{x, y + 100, z},
		{x, y + 100, z + 100},
		{x, y, z + 100},
	};
}

TArray<FVector> Block::GetBackVertecies(float x, float y, float z)
{
	return {
		{x + 100, y + 100, z},
		{x + 100, y, z},
		{x + 100, y, z + 100},
		{x + 100, y + 100, z + 100},
	};
}

TArray<int32> Block::GetTopTrianglesFrom(int32 Start)
{
	return {
		Start, Start + 1 , Start + 2,
		Start, Start + 2, Start + 3
	};
}


TArray<int32> Block::GetRightTrianglesFrom(int32 Start)
{
	return {
		Start, Start + 1 , Start + 2,
		Start, Start + 2, Start + 3
	};
}

TArray<int32> Block::GetBottomTrianglesFrom(int32 Start)
{
	return {
		Start, Start + 1 , Start + 2,
		Start, Start + 2, Start + 3
	};
}

TArray<int32> Block::GetLeftTrianglesFrom(int32 Start)
{
	return {
		Start, Start + 1 , Start + 2,
		Start, Start + 2, Start + 3
	};
}

TArray<int32> Block::GetFrontTrianglesFrom(int32 Start)
{
	return {
		Start, Start + 1 , Start + 2,
		Start, Start + 2, Start + 3
	};
}

TArray<int32> Block::GetBackTrianglesFrom(int32 Start)
{
	return {
		Start, Start + 1 , Start + 2,
		Start, Start + 2, Start + 3
	};
}

TArray<FVector2D> Block::GetTopUVs()
{
	return {
		{0, 0},
		{1, 0},
		{1, 1},
		{0, 1}
	};
}

TArray<FVector2D> Block::GetRightUVs()
{
	return {
		{0, 0},
		{1, 0},
		{1, 1},
		{0, 1}
	};
}

TArray<FVector2D> Block::GetBottomUVs()
{
	return {
		{0, 1},
		{0, 0},
		{1, 0},
		{1, 1}
	};
}

TArray<FVector2D> Block::GetLeftUVs()
{
	return {
		{1, 0},
		{1, 1},
		{0, 1},
		{0, 0}
	};
}

TArray<FVector2D> Block::GetFrontUVs()
{
	return {
		{0, 0},
		{1, 0},
		{1, 1},
		{0, 1}
	};
}

TArray<FVector2D> Block::GetBackUVs()
{
	return {
		{0, 0},
		{1, 0},
		{1, 1},
		{0, 1}
	};
}

TArray<FVector> Block::GetTopNormals()
{
	return {
		{0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}
	};
}

TArray<FVector> Block::GetRightNormals()
{
	return {
		{0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}
	};
}

TArray<FVector> Block::GetBottomNormals()
{
	return {
		{0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}
	};
}

TArray<FVector> Block::GetLeftNormals()
{
	return {
		{0, -1, 0}, {0, -1, 0}, {0, -1, 0}, {0, -1, 0}
	};
}

TArray<FVector> Block::GetFrontNormals()
{
	return {
		{-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}
	};
}

TArray<FVector> Block::GetBackNormals()
{
	return {
		{1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}
	};
}

TArray<FVector> Block::GetAllVertecies(float x, float y, float z)
{
	return {
		{x, y, z + 100},
		{x, y + 100, z + 100},
		{x + 100, y + 100, z + 100},
		{x + 100, y, z + 100},
		{x, y + 100, z},
		{x + 100, y + 100, z},
		{x + 100, y + 100, z + 100},
		{x, y + 100, z + 100},
		{x, y, z},
		{x + 100, y, z},
		{x + 100, y + 100, z},
		{x, y + 100, z},
		{x, y, z},
		{x, y, z + 100},
		{x + 100, y, z + 100},
		{x + 100, y, z},
		{x, y, z},
		{x, y + 100, z},
		{x, y + 100, z + 100},
		{x, y, z + 100},
		{x + 100, y + 100, z},
		{x + 100, y, z},
		{x + 100, y, z + 100},
		{x + 100, y + 100, z + 100},
	};
}

TArray<FVector2D> Block::GetAllUVs()
{
	return {
		{0, 0},
		{1, 0},
		{1, 1},
		{0, 1},

		{0, 0},
		{1, 0},
		{1, 1},
		{0, 1},

		{0, 1},
		{0, 0},
		{1, 0},
		{1, 1},

		{1, 0},
		{1, 1},
		{0, 1},
		{0, 0},

		{0, 0},
		{1, 0},
		{1, 1},
		{0, 1},

		{0, 0},
		{1, 0},
		{1, 1},
		{0, 1}
	};
}

TArray<FVector> Block::GetAllNormals()
{
	return {
		{0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1},
		{0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0},
		{0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1},
		{0, -1, 0}, {0, -1, 0}, {0, -1, 0}, {0, -1, 0},
		{-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0},
		{1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}
	};
}

TArray<int32> Block::GetAllTrainglesFrom(int32 Start)
{
	TArray<int32> Tris;
	Tris.Reserve(36);
	for (int face = 0; face < 6; face++)
	{
		int vl = face * 4 + Start;
		Tris.Append({
			vl, vl + 1 , vl + 2,
			vl, vl + 2, vl + 3
			});
	};
	return Tris;
}
