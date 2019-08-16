

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Item.h"
#include "ItemUEWrapper.h"
#include "ItemStack.generated.h"


USTRUCT(BlueprintType)
struct FItemStack {
	GENERATED_BODY()

	//UPROPERTY()
	Item* ItemS;
	UPROPERTY(BlueprintReadOnly)
	int32 ItemCount;

	FItemStack() : ItemS(nullptr), ItemCount(0) {}
	FItemStack(Item* I, int32 Num) : ItemS(I), ItemCount(Num) {}
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
