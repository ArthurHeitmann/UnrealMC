

#pragma once

#include "CoreMinimal.h"
#include "Item.h"

/**
 * 
 */
class MC_FAKE_API I_NoItem : public Item
{
public:
	I_NoItem();

	virtual bool IsStackableWith(Item* OtherItem) override;
};
