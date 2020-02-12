#include "I_BlockItem.h"
#include "../Blocks/B_Block.h"
#include "Misc/McStaticFunctions.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/World.h"
#include "Player/BlockSpaceChecker.h"

I_BlockItem::I_BlockItem(class B_Block* Block)
{
	this->BlockRef = Block;
	Texture = Block->GetTexture();
	Id = Block->GetBlockID();
	ItemName = Block->GetName();
	bCustomDisplayMesh = true;
}

I_Item::PostUseTask I_BlockItem::OnItemUse(const FHitResult& HitPointData, AMcWorld* World)
{
	int32 x = floorf(HitPointData.Location.X / 100.f);
	int32 y = floorf(HitPointData.Location.Y / 100.f);
	int32 z = HitPointData.Location.Z / 100;
	if (HitPointData.ImpactNormal.Z < 0)
			--z;
		else if (HitPointData.ImpactNormal.X < 0)
			--x;
		else if (HitPointData.ImpactNormal.Y < 0)
			--y;

	 ABlockSpaceChecker* SpaceChecker = World->GetWorld()->SpawnActor<ABlockSpaceChecker>(FVector{ x * 100.f, y * 100.f, z * 100.f }, FRotator::ZeroRotator, 
		 FActorSpawnParameters());

	 if (SpaceChecker->bHitPlayer)
	 {
		 SpaceChecker->Destroy();
		 return { NoTask, 0 };
	 }
	 else
	 {
		 SpaceChecker->Destroy();
		 World->AddBlockSetTask(x, y, z, BlockRef->Clone(), 255);
		 return { Decrement, 1 };
	 }
}

void I_BlockItem::GetCustomDisplayMesh(UObject* Base, TArray<FVector>& Verts, TArray<FVector2D>& UVs, TArray<int32>& Tris, TArray<FVector>& Normals, UMaterial*& Mat)
{
	Verts = BlockRef->GetAllVertices(0, 0, 0);
	UVs = BlockRef->GetAllUVs();
	Tris = BlockRef->GetAllTrianglesFrom();
	Normals = BlockRef->GetAllNormals();
	Mat = (UMaterial*) BlockRef->GetMaterial(Base);
}

I_BlockItem::~I_BlockItem()
{
	if (BlockRef->GetName() != "Air")
		delete BlockRef;
}
