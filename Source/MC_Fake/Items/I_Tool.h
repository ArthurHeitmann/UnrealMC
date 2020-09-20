

#pragma once

#include "CoreMinimal.h"
#include "I_Item.h"

/**
 * 
 */
class MC_FAKE_API I_Tool : public I_Item
{
public:
	I_Tool();

	int32 GetDurability();
	int32 GetCharges();

	virtual bool IsStackableWith(I_Item* Item) override;

protected:
	int32 Durability;
	int32 Charges;
};
