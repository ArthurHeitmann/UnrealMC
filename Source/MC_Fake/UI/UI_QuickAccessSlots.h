#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Items/ItemStack.h"
#include "UI_ItemSlot.h"
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
	class UCanvasPanel* Root;
	class UItemSlots* Inventory;
	UPROPERTY(EditAnywhere)
	UUI_ItemSlot* First;
	TArray<ItemSlotCombo> UIItemSlots;

	const float DEG_TO_RAD = 0.0174532925199;
	float DistFromCenter;

public:
	bool Initialize();

	/*FItemStack& SetSelecteddItemSlotById(int32 id);
	FItemStack& NextSelectedItem();
	FItemStack& PreviousSelectedItem();*/

	void SetInventorySlots(class UItemSlots* NewInventorySlots, int32 pSlotsPerRow = 10);
};