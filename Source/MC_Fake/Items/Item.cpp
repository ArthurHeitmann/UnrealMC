


#include "Item.h"

Item::Item()
{
	BreakingSpeed = 1.f;
	AttackDamage = 1.f;
	Strength = 0;
	ItemEnum = INoItem;
	Texture = nullptr;
}

TSet<TEnumAsByte<EItemActions>> Item::GetItemActions()
{
	return ItemActions;
}

TEnumAsByte<EAllItems> Item::GetItemEnum()
{
	return ItemEnum;
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

bool Item::IsStackableWith(Item* OtherItem)
{
	return ItemEnum == OtherItem->GetItemEnum();
}

Item::PostUseTask Item::OnItemUse(FHitResult HitPointData, AMcWorld* World)
{
	return { NoTask };
}

UTexture* Item::GetTexture()
{
	return Texture;
}
