

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../../Items/ItemStack.h"
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
	UPROPERTY(EditAnywhere)
	TArray<FItemStack> Slots;

public:
	UItemSlots();

	virtual FItemStack& GetStackAt(int x);
	virtual void SetNumSlots(int Num);
	virtual int32 GetNumSlots();
	FItemStack PickupItemStack(FItemStack Items);
	void UpdateSlotsUI(UWorld* world);

	virtual ~UItemSlots() override;
};
