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