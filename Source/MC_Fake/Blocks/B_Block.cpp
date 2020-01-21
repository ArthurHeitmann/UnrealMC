#include "B_Block.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"
#include "../Items/Item.h"
#include "../Items/ItemDrop.h"
#include "../Items/I_BlockItem.h"
#include "Materials/MaterialInstanceDynamic.h"


UMaterial* B_Block::BlockMaterial;

B_Block::B_Block()
{
	BlockID = -1;
	SubID = 0;
	BlockName = TEXT("No Name Block");
	BlockModelType = BLOCK;
	BreakingAction = ANoAction;
	BreakTime = 0.75f;	
	if (!B_Block::BlockMaterial)
	{
		static ConstructorHelpers::FObjectFinder<UMaterial> MaterialInstanceObj(TEXT("Material'/Game/Materials/Blocks/M_BlockDefault.M_BlockDefault'"));
		B_Block::BlockMaterial = MaterialInstanceObj.Object;
	}
	bCustomItemDrops = false;
	bCustomCollisionMesh = false;

	Texture = nullptr;
	TextureBMP = nullptr;
}

B_Block::~B_Block()
{
}

B_Block* B_Block::GetBlockFromBytes(uint8* Bytes)
{
	/*uint16* id = reinterpret_cast<uint16*>(Bytes);
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
	}*/
	return nullptr;
}

void B_Block::DropItem(UWorld* World, FVector Location, Item* DropItem)
{
	AItemDrop* Drop = World->SpawnActor<AItemDrop>(Location + FVector(50, 50, 50), FRotator::ZeroRotator);
	if (!Drop)
		return;
	Drop->SetItemStack({ DropItem, 1 });
	Drop->SetMesh(GetAllVertices(-50, -50, -50), GetAllTrianglesFrom(0), GetAllUVs(), GetAllNormals(), GetMaterial(Drop));
}

bool B_Block::IsSideOptimizable(EDirection Direction)
{
	return true;
}

bool B_Block::IsBlockOpaque()
{
	return true;
}

bool B_Block::UsesCustomModel()
{
	return false;
}

FName B_Block::GetName()
{
	return BlockName;
}

float B_Block::GetBreakTime()
{
	return BreakTime;
}

B_Block* B_Block::Clone()
{
	UE_LOG(LogTemp, Warning, TEXT("Cloning default Block"));
	return new B_Block(*this);
}

TArray<uint8> B_Block::GetBinaryData()
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

bool B_Block::operator==(const B_Block& B)
{
	return BlockName == B.BlockName && BlockEnum == B.BlockEnum;
}

bool B_Block::operator!=(const B_Block& B)
{
	return !(*this == B);
}

void B_Block::OnPlace(class UWorld* World, FVector Location)
{
}

void B_Block::OnInteract(class UWorld* World, FVector Location)
{
}

void B_Block::OnBeginBreak(class UWorld* World, FVector Location)
{
}

void B_Block::UpdateBreak(class UWorld* World, FVector Location, float Seconds)
{
}

void B_Block::OnBreak(class UWorld* World, FVector Location)
{
	if (bCustomItemDrops)
	{
		for (const auto& DropProb : PossibleItemDrops)
		{
			if (FMath::FRandRange(0.f, 1.f) >= DropProb.Chance)
				DropItem(World, Location, DropProb.Item);
		}
	}
	else
		DropItem(World, Location, new I_BlockItem(this));
}

EBlockModelType B_Block::GetBlockModelType()
{
	return BlockModelType;
}

EAllBlocks B_Block::GetBlockEnum()
{
	return BlockEnum;
}

EItemActions B_Block::GetBreakingAction()
{
	return BreakingAction;
}

UMaterialInstanceDynamic* B_Block::GetMaterial(UObject* UObj)
{
	UMaterialInstanceDynamic* OutMat = UMaterialInstanceDynamic::Create(BlockMaterial, UObj);
	if (Texture)
		OutMat->SetTextureParameterValue(TEXT("Block Texture"), Texture);
	if (TextureBMP)
		OutMat->SetTextureParameterValue(TEXT("Bump Texture"), TextureBMP);
	return OutMat;
}

UTexture* B_Block::GetTexture()
{
	return Texture;
}

UTexture* B_Block::GetTextureBMP()
{
	return TextureBMP;
}

TArray<FVector> B_Block::GetTopVertices(float x, float y, float z)
{
	return {
		{x, y, z + 100},
		{x, y + 100, z + 100},
		{x + 100, y + 100, z + 100},
		{x + 100, y, z + 100},
	};
}

TArray<FVector> B_Block::GetRightVertices(float x, float y, float z)
{
	return {
		{x, y + 100, z},
		{x + 100, y + 100, z},
		{x + 100, y + 100, z + 100},
		{x, y + 100, z + 100},
	};
}

TArray<FVector> B_Block::GetBottomVertices(float x, float y, float z)
{
	return {
		{x, y, z},
		{x + 100, y, z},
		{x + 100, y + 100, z},
		{x, y + 100, z},
	};
}

TArray<FVector> B_Block::GetLeftVertices(float x, float y, float z)
{
	return {
		{x, y, z},
		{x, y, z + 100},
		{x + 100, y, z + 100},
		{x + 100, y, z},
	};
}

TArray<FVector> B_Block::GetFrontVertices(float x, float y, float z)
{
	return {
		{x, y, z},
		{x, y + 100, z},
		{x, y + 100, z + 100},
		{x, y, z + 100},
	};
}

TArray<FVector> B_Block::GetBackVertices(float x, float y, float z)
{
	return {
		{x + 100, y + 100, z},
		{x + 100, y, z},
		{x + 100, y, z + 100},
		{x + 100, y + 100, z + 100},
	};
}

TArray<int32> B_Block::GetTopTrianglesFrom(int32 Start)
{
	return {
		Start, Start + 1 , Start + 2,
		Start, Start + 2, Start + 3
	};
}


TArray<int32> B_Block::GetRightTrianglesFrom(int32 Start)
{
	return {
		Start, Start + 1 , Start + 2,
		Start, Start + 2, Start + 3
	};
}

TArray<int32> B_Block::GetBottomTrianglesFrom(int32 Start)
{
	return {
		Start, Start + 1 , Start + 2,
		Start, Start + 2, Start + 3
	};
}

TArray<int32> B_Block::GetLeftTrianglesFrom(int32 Start)
{
	return {
		Start, Start + 1 , Start + 2,
		Start, Start + 2, Start + 3
	};
}

TArray<int32> B_Block::GetFrontTrianglesFrom(int32 Start)
{
	return {
		Start, Start + 1 , Start + 2,
		Start, Start + 2, Start + 3
	};
}

TArray<int32> B_Block::GetBackTrianglesFrom(int32 Start)
{
	return {
		Start, Start + 1 , Start + 2,
		Start, Start + 2, Start + 3
	};
}

TArray<FVector2D> B_Block::GetTopUVs()
{
	return {
		{0, 0},
		{1, 0},
		{1, 1},
		{0, 1}
	};
}

TArray<FVector2D> B_Block::GetRightUVs()
{
	return {
		{0, 0},
		{1, 0},
		{1, 1},
		{0, 1}
	};
}

TArray<FVector2D> B_Block::GetBottomUVs()
{
	return {
		{0, 1},
		{0, 0},
		{1, 0},
		{1, 1}
	};
}

TArray<FVector2D> B_Block::GetLeftUVs()
{
	return {
		{1, 0},
		{1, 1},
		{0, 1},
		{0, 0}
	};
}

TArray<FVector2D> B_Block::GetFrontUVs()
{
	return {
		{0, 0},
		{1, 0},
		{1, 1},
		{0, 1}
	};
}

TArray<FVector2D> B_Block::GetBackUVs()
{
	return {
		{0, 0},
		{1, 0},
		{1, 1},
		{0, 1}
	};
}

TArray<FVector> B_Block::GetTopNormals()
{
	return {
		{0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}
	};
}

TArray<FVector> B_Block::GetRightNormals()
{
	return {
		{0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}
	};
}

TArray<FVector> B_Block::GetBottomNormals()
{
	return {
		{0, 0, -1}, {0, 0, -1}, {0, 0, -1}, {0, 0, -1}
	};
}

TArray<FVector> B_Block::GetLeftNormals()
{
	return {
		{0, -1, 0}, {0, -1, 0}, {0, -1, 0}, {0, -1, 0}
	};
}

TArray<FVector> B_Block::GetFrontNormals()
{
	return {
		{-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}
	};
}

TArray<FVector> B_Block::GetBackNormals()
{
	return {
		{1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}
	};
}

TArray<FVector> B_Block::GetAllVertices(float x, float y, float z)
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

TArray<FVector2D> B_Block::GetAllUVs()
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

TArray<FVector> B_Block::GetAllNormals()
{
	return {
		{0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1},
		{0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0},
		{0, 0, -1}, {0, 0, -1}, {0, 0, -1}, {0, 0, -1},
		{0, -1, 0}, {0, -1, 0}, {0, -1, 0}, {0, -1, 0},
		{-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0},
		{1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}
	};
}

TArray<int32> B_Block::GetAllTrianglesFrom(int32 Start)
{
	TArray<int32> Triangles;
	Triangles.Reserve(36);
	for (int face = 0; face < 6; ++face)
	{
		int vl = face * 4 + Start;
		Triangles.Append({
			vl, vl + 1 , vl + 2,
			vl, vl + 2, vl + 3
			});
	};
	return Triangles;
}
