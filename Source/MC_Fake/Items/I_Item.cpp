#include "I_Item.h"

I_Item::I_Item()
{
	BreakingSpeed = 1.f;
	AttackDamage = 1.f;
	Strength = 0;
	MaxStackCount = 64;
	bIsStackable = true;
	Texture = nullptr;
	bCustomDisplayMesh = false;
}

FName I_Item::GetName()
{
	return ItemName;
}

uint16 I_Item::GetId()
{
	return Id;
}

TSet<EItemActions> I_Item::GetItemActions()
{
	return ItemActions;
}

float I_Item::GetAttackDamage()
{
	return AttackDamage;
}

float I_Item::GetBreakingSpeed()
{
	return BreakingSpeed;
}

int I_Item::GetStrength()
{
	return Strength;
}

bool I_Item::IsItemStackable()
{
	return bIsStackable;
}

int32 I_Item::GetMaxStackCount()
{
	return MaxStackCount;
}

bool I_Item::IsStackableWith(I_Item* OtherItem)
{
	return ItemName == OtherItem->GetName();
}

bool I_Item::HasCustomDisplayMesh()
{
	return bCustomDisplayMesh;
}

void I_Item::GetCustomDisplayMesh(UObject* Base, TArray<FVector>& VerticesOut, TArray<FVector2D>& UVsOut, TArray<int32>& TrianglesOut, TArray<FVector>& NormalsOut, UMaterial *& MatOut)
{
}

bool I_Item::operator==(const I_Item& I)
{
	return ItemName == I.ItemName;
}

bool I_Item::operator!=(const I_Item& I)
{
	return !(*this == I);
}

I_Item::PostUseTask I_Item::OnItemUse(const FHitResult& HitPointData, AMcWorld* World)
{
	return { NoTask };
}

UTexture* I_Item::GetTexture()
{
	return Texture;
}

I_Item::~I_Item()
{
}
