


#include "Item.h"

Item::Item()
{
	BreakingSpeed = 1.f;
	AttackDamage = 1.f;
	Strength = 0;
}

TSet<TEnumAsByte<EItemActions>> Item::GetItemActions()
{
	return ItemActions;
}

float Item::GetAttackDamage()
{
	return AttackDamage;
}

float Item::GetBreakingSpeed()
{
	return BreakingSpeed;
}

int Item::GetStrength()
{
	return Strength;
}
