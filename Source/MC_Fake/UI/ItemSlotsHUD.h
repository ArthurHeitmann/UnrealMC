

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Items/ItemStack.h"
#include "ItemSlot.h"
#include "ItemSlotsHUD.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class MC_FAKE_API UItemSlotsHUD : public UUserWidget
{
	GENERATED_BODY()
	
private:
	class UHorizontalBox* SlotContainer;
	class UCanvasPanelSlot* ContentSlot;
	TArray<UItemSlot*> Slots;
	virtual void NativeConstruct() override;

public:
	virtual bool Initialize() override;

	UFUNCTION(BlueprintCallable)
	void SetSize(int32 Count);
	UFUNCTION(BlueprintCallable)
	void SetSlotItemStack(int32 SlotId, const FItemStack& NewItemStack);
};
