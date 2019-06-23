

#pragma once

#include "CoreMinimal.h"
#include "Item.h"

/**
 * 
 */
class MC_FAKE_API I_Tool : public Item
{
public:
	I_Tool();

	int32 GetDurability();
	int32 GetCharges();

	virtual bool IsStackableWith(Item* Item) override;

protected:
	int32 Durability;
	int32 Charges;
};
