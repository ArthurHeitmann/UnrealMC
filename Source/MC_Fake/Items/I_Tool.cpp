


#include "I_Tool.h"

I_Tool::I_Tool()
{
}

int32 I_Tool::GetDurability()
{
	return Durability;
}

int32 I_Tool::GetCharges()
{
	return Charges;
}

bool I_Tool::IsStackableWith(Item* Item)
{
	return false;
}
