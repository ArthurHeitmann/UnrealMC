

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
	TEnumAsByte<EAllItems> ItemEnum;
	TSet<TEnumAsByte<EItemActions>> ItemActions;
	float AttackDamage;
	float BreakingSpeed;
	int Strength;
	UTexture* Texture;

public:
	Item();

	TSet<TEnumAsByte<EItemActions>> GetItemActions();
	TEnumAsByte<EAllItems> GetItemEnum();
	float GetAttackDamage();
	float GetBreakingSpeed();
	int GetStrength();
	virtual bool IsStackableWith(Item* OtherItem);

	virtual PostUseTask OnItemUse(FHitResult HitPointData, class AMcWorld* World);

	class UTexture* GetTexture();
};
