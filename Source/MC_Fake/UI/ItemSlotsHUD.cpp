


#include "ItemSlotsHUD.h"
#include "WidgetTree.h"
#include "HorizontalBox.h"
#include "HorizontalBoxSlot.h"
#include "CanvasPanel.h"
#include "CanvasPanelSlot.h"
#include "../Items/I_NoItem.h"

void UItemSlotsHUD::NativeConstruct()
{
	Super::NativeConstruct();
}

bool UItemSlotsHUD::Initialize()
{
	if (Super::Initialize() == false)
		return false;

	return true;
}

void UItemSlotsHUD::SetSize(int32 Count)
{
}

void UItemSlotsHUD::SetSlotItemStack(int32 SlotId, const FItemStack& NewItemStack)
{
}
