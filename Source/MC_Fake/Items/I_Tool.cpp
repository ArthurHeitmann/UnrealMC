


#include "I_Tool.h"

I_Tool::I_Tool()
{
	bIsStackable = false;
	MaxStackCount = 1;
}

int32 I_Tool::GetDurability()
{
	return Durability;
}

int32 I_Tool::GetCharges()
{
	return Charges;
}

bool I_Tool::IsStackableWith(I_Item* Item)
{
	return false;
}
