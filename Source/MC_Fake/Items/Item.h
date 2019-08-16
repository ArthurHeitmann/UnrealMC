

#pragma once

#include "CoreMinimal.h"
#include "Enums.h"
#include "Engine/EngineTypes.h"

/**
 * 
 */
class MC_FAKE_API Item
{

public:
	struct PostUseTask {
		TEnumAsByte<EPostItemUseTask> Tasks;
		int32 Count;
	};

protected:
	FName ItemName;
	int32 Id;
	TEnumAsByte<EAllItems> ItemEnum;
	TSet<TEnumAsByte<EItemActions>> ItemActions;
	float AttackDamage;
	float BreakingSpeed;
	int32 Strength;
	bool bIsStackable;
	int32 MaxStackCount;
	UTexture* Texture;

public:
	Item();

	FName GetName();
	int32 GetId();
	TSet<TEnumAsByte<EItemActions>> GetItemActions();
	TEnumAsByte<EAllItems> GetItemEnum();
	float GetAttackDamage();
	float GetBreakingSpeed();
	int GetStrength();
	bool IsItemStackable();
	int32 GetMaxStackCount();
	virtual bool IsStackableWith(Item* OtherItem);

	virtual PostUseTask OnItemUse(const FHitResult& HitPointData, class AMcWorld* World);

	class UTexture* GetTexture();

	virtual ~Item();
};
