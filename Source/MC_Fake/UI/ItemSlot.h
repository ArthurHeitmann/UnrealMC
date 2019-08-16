

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Items/ItemStack.h"
#include "ItemSlot.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class MC_FAKE_API UItemSlot : public UUserWidget
{
	GENERATED_BODY()
	
private:
	class UTextBlock* NameText;
	class UTextBlock* NumberText;

	virtual void NativeConstruct() override;
public:
	virtual bool Initialize() override;

	UPROPERTY(BlueprintReadOnly)
	FItemStack ItemStackC;

	UFUNCTION(BlueprintCallable)
	void SetItemStack(const FItemStack& NewItemStack);
};
