

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../../Items/ItemStack.h"
#include "ItemSlots.generated.h"


/**
 * 
 */
UCLASS()
class MC_FAKE_API UItemSlots : public UObject
{
	GENERATED_BODY()
	
protected:
	TArray<FItemStack*> Slots;

public:
	UItemSlots();

	virtual FItemStack* GetStackAt(int x);
	virtual void SetNumSlots(int Num);
	virtual int32 GetNumSlots();
	FItemStack PickupItemStack(FItemStack Items);

	virtual ~UItemSlots() override;
};
