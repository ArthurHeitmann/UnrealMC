

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../Items/ItemStack.h"
#include "ItemSlots.generated.h"


//USTRUCT()
//struct FItemStack {
//	GENERATED_BODY()
//
//	int32 Number;
//	Item* Item;
//};

/**
 * 
 */
UCLASS()
class MC_FAKE_API UItemSlots : public UObject
{
	GENERATED_BODY()
	
protected:
	TArray<FItemStack> Slots;


public:
	UItemSlots();

	virtual FItemStack GetStackAt(int x);
	virtual void SetNumSlots(int Num);
	FItemStack PickupItemStack(FItemStack Items);
	void DebugPrint(UWorld* world);

	virtual ~UItemSlots() override;
};
