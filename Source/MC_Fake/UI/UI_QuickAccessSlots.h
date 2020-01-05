#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Items/ItemStack.h"
#include "UI_ItemSlot.h"
#include "Components/TimelineComponent.h"
#include "UI_QuickAccessSlots.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MC_FAKE_API UUI_QuickAccessSlots : public UUserWidget
{
	GENERATED_BODY()

private:
	struct ItemSlotCombo {
		UUI_ItemSlot* InvSlot;
		class UCanvasPanelSlot* UISlot;
	};

	int32 SelectedItemId;
	int32 SlotsPerRow;
	int32 SelectedRowID;
	FItemStack const** SelectedItem;
	UPROPERTY()
	class UCanvasPanel* Root;
	UPROPERTY()
	class UItemSlots* Inventory;
	
	TArray<ItemSlotCombo> UIItemSlots;


	static const float DEG_TO_RAD;
	float DistFromCenter;

	bool bIsVisible;

protected:

public:
	UUI_QuickAccessSlots(const FObjectInitializer& FOs);
	bool Initialize() override;

	void SetInventorySlots(class UItemSlots* NewInventorySlots, int32 pSlotsPerRow = 10);
	void SetSelectedItemPointer(FItemStack const** ISP);
	void ToggleVisibility();

	void SelectItemStack(int32 id);
	void SelectNextItem();
	void SelectPreviousItem();
};