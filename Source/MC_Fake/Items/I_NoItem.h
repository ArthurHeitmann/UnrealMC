

#pragma once

#include "CoreMinimal.h"
#include "I_Item.h"

/**
 * 
 */
class MC_FAKE_API I_NoItem : public I_Item
{
public:
	I_NoItem();

	virtual bool IsStackableWith(I_Item* OtherItem) override;
};
