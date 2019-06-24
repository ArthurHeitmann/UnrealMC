#include "I_BlockItem.h"
#include "../Blocks/Block.h"

I_BlockItem::I_BlockItem(class Block* Block)
{
	this->Block = Block;
	Texture = Block->GetTexture();

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

Item::PostUseTask I_BlockItem::OnItemUse(FHitResult HitPointData, AMcWorld* World)
{

	int32 x = HitPointData.Location.X / 100;
	int32 y = HitPointData.Location.Y / 100;
	int32 z = HitPointData.Location.Z / 100;
	bool NegXApplied = false;
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
	}

	/*	if (HitPointData.ImpactNormal.Z > 0)
			z++;
		else*/ if (HitPointData.ImpactNormal.Z < 0)
			z--;
		else if (HitPointData.ImpactNormal.X > 0)
			x++;
		else if (HitPointData.ImpactNormal.X < 0 && !NegXApplied)
			x--;
		else if (HitPointData.ImpactNormal.Y > 0)
			y++;
		else if (HitPointData.ImpactNormal.Y < 0 && !NegYApplied)
			y--;
	World->AddBlockSetTask(x, y, z, Block, 255);

	return { Decrement, 1 };
}
