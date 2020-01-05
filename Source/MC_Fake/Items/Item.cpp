#include "Item.h"

Item::Item()
{
	BreakingSpeed = 1.f;
	AttackDamage = 1.f;
	Strength = 0;
	MaxStackCount = 64;
	bIsStackable = true;
	ItemEnum = INoItem;
	Texture = nullptr;
	bCustomDisplayMesh = false;
}

FName Item::GetName()
{
	return ItemName;
}

uint16 Item::GetId()
{
	return Id;
}

TSet<EItemActions> Item::GetItemActions()
{
	return ItemActions;
}

EAllItems Item::GetItemEnum()
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

bool Item::IsItemStackable()
{
	return bIsStackable;
}

int32 Item::GetMaxStackCount()
{
	return MaxStackCount;
}

bool Item::IsStackableWith(Item* OtherItem)
{
	return ItemEnum == OtherItem->GetItemEnum();
}

bool Item::HasCustomDisplayMesh()
{
	return bCustomDisplayMesh;
}

void Item::GetCustomDisplayMesh(UObject* Base, TArray<FVector>& VerticesOut, TArray<FVector2D>& UVsOut, TArray<int32>& TrianglesOut, TArray<FVector>& NormalsOut, UMaterial *& MatOut)
{
}

bool Item::operator==(const Item& I)
{
	return ItemName == I.ItemName && ItemEnum == I.ItemEnum;
}

bool Item::operator!=(const Item& I)
{
	return !(*this == I);
}

Item::PostUseTask Item::OnItemUse(const FHitResult& HitPointData, AMcWorld* World)
{
	return { NoTask };
}

UTexture* Item::GetTexture()
{
	return Texture;
}

Item::~Item()
{
}
