

#pragma once

#include "CoreMinimal.h"
#include "Enums.h"

/**
 * 
 */
class MC_FAKE_API Item
{
private:
	TSet<TEnumAsByte<EItemActions>> ItemActions;
	float AttackDamage;
	float BreakingSpeed;
	int Strength;

public:
	Item();

	TSet<TEnumAsByte<EItemActions>> GetItemActions();
	float GetAttackDamage();
	float GetBreakingSpeed();
	int GetStrength();
};
