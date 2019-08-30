#pragma once
#include "CoreMinimal.h"
#include "ItemSlots.h"
#include "QuickAccesSlots.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MC_FAKE_API UQuickAccesSlots : public UItemSlots
{
	GENERATED_BODY()

private:
	int32 SelectedItemId;
	
public:
	UQuickAccesSlots();

	FItemStack& SetSelecteddItemSlotById(int32 id);

	FItemStack& NextSelectedItem();

	FItemStack& PreviousSelectedItem();

};