

#pragma once

#include "CoreMinimal.h"
#include "Enums.h"

/**
 * 
 */
class MC_FAKE_API Item
{
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

	class UTexture* GetTexture();
};
