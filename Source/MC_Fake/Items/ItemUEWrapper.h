

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemUEWrapper.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class MC_FAKE_API UItemUEWrapper : public UObject
{
	GENERATED_BODY()
	
private:
	class I_Item* ItemRef;

public:
	UFUNCTION(BlueprintCallable)
	FName GetItemName();
	UFUNCTION(BlueprintCallable)
	int32 GetItemId();

	void SetItem(class I_Item* NewItem);

};
