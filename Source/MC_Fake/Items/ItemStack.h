

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Item.h"
#include "ItemUEWrapper.h"
#include "I_NoItem.h"
#include "ItemStack.generated.h"


USTRUCT(BlueprintType)
struct FItemStack {
	GENERATED_BODY()

	//UPROPERTY()
	UPROPERTY(BlueprintReadOnly)
	int32 ItemCount;
	Item* ItemS;

	FItemStack() : ItemCount(0), ItemS(nullptr) {}
	FItemStack(Item* I, int32 Num) : ItemCount(Num), ItemS(I) {}
	~FItemStack() {
		UE_LOG(LogTemp, Warning, TEXT(""));
	}
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