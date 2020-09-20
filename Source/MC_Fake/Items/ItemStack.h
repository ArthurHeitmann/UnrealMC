

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "I_Item.h"
#include "ItemUEWrapper.h"
#include "I_NoItem.h"
#include "ItemStack.generated.h"


USTRUCT(BlueprintType)
struct FItemStack {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 ItemCount;
	I_Item* ItemS;

	FItemStack() : ItemCount(0), ItemS(nullptr) {}
	FItemStack(I_Item* I, int32 Num) : ItemCount(Num), ItemS(I) {}
	~FItemStack() {}
};


/**
 * 
 */
UCLASS()
class MC_FAKE_API UItemStackHelper : public UObject
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable)
	static UItemUEWrapper* ItemToItemWrapper(FItemStack I);
};
