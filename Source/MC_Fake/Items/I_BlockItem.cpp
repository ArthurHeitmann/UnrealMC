#include "I_BlockItem.h"
#include "../Blocks/B_Block.h"
#include "Misc/McStaticFunctions.h"
#include "Materials/MaterialInstanceDynamic.h"

I_BlockItem::I_BlockItem(class B_Block* Block)
{
	this->BlockRef = Block;
	Texture = Block->GetTexture();
	Id = Block->GetBlockEnum();
	ItemName = Block->GetName();
	bCustomDisplayMesh = true;

	switch (Block->GetBlockEnum())
	{
	case BStone:
		ItemEnum = IStone;
		break;
	case BDirt:
		ItemEnum = IDirt;
		break;
	case BGrass:
		ItemEnum = IGrass;
		break;
	case BLog_Oak:
		ItemEnum = ILog_Oak;
		break;

	default:
		ItemEnum = INoItem;
	}

}

Item::PostUseTask I_BlockItem::OnItemUse(const FHitResult& HitPointData, AMcWorld* World)
{
	int32 x = floorf(HitPointData.Location.X / 100.f);
	int32 y = floorf(HitPointData.Location.Y / 100.f);
	int32 z = HitPointData.Location.Z / 100;
	/*bool NegXApplied = false;
	bool NegYApplied = false;
	if (HitPointData.Location.X < 0)
	{
		x--;
		NegXApplied = true;
	}
	if (HitPointData.ImpactPoint.Y < 0)
	{
		y--;
		NegYApplied = true;
	}*/

	/*	if (HitPointData.ImpactNormal.Z > 0)
			z++;
		else*/ if (HitPointData.ImpactNormal.Z < 0)
			--z;
		/*else if (HitPointData.ImpactNormal.X > 0)
			x++;*/
		else if (HitPointData.ImpactNormal.X < 0/* && !NegXApplied*/)
			--x;
		/*else if (HitPointData.ImpactNormal.Y > 0)
			y++;*/
		else if (HitPointData.ImpactNormal.Y < 0/* && !NegYApplied*/)
			--y;
	World->AddBlockSetTask(x, y, z, BlockRef->Clone(), 255);

	return { Decrement, 1 };
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
	if (BlockRef->GetBlockEnum() != BAir)
		delete BlockRef;
}
