

#pragma once

#include "CoreMinimal.h"
#include "Item.h"

/**
 * 
 */
class MC_FAKE_API I_BlockItem : public Item
{
private:
	class Block* Block;

public:
	I_BlockItem(class Block* Block);
	virtual Item::PostUseTask OnItemUse(const FHitResult& HitPointData, class AMcWorld* World) override;
};
