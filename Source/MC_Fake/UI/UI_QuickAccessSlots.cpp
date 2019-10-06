


#include "UI_QuickAccessSlots.h"
#include "../Player/Inventory/ItemSlots.h"
#include "CanvasPanel.h"
#include "CanvasPanelSlot.h"
#include "WidgetTree.h"
#include "TextBlock.h"

bool UUI_QuickAccessSlots::Initialize()
{
	if (Super::Initialize() == false)
		return false;

	SelectedItemId = 0;
	SelectedRowID = 0;
	DistFromCenter = 350;

	Root = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("QuAcSl Root Canvas"));
	WidgetTree->RootWidget = Root;
	
	return true;
}

/*void UUI_QuickAccessSlots::SetSelecteddItemPointer(Item** NewItemPointer)
{
	SelectedItem = NewItemPointer;
}*/

//FItemStack& UUI_QuickAccessSlots::SetSelecteddItemSlotById(int32 id)
//{
//	if (id < Slots.Num() && id >= 0)
//		SelectedItemId = id;
//	else if (id >= 0)
//		SelectedItemId = id % Slots.Num();
//	else
//		SelectedItemId = -id % Slots.Num();
//
//	//*SelectedItem = Slots[SelectedItemId].ItemS;
//	return Slots[SelectedItemId];
//}
//
//FItemStack& UUI_QuickAccessSlots::NextSelectedItem()
//{
//	SelectedItemId = ++SelectedItemId % Slots.Num();
//	//*SelectedItem = Slots[SelectedItemId].ItemS;
//	return Slots[SelectedItemId];
//}
//
//FItemStack& UUI_QuickAccessSlots::PreviousSelectedItem()
//{
//	SelectedItemId = --SelectedItemId % Slots.Num();
//	//*SelectedItem = Slots[SelectedItemId].ItemS;
//
//	return Slots[SelectedItemId];
//}

void UUI_QuickAccessSlots::SetInventorySlots(UItemSlots* NewInventorySlots, int32 pSlotsPerRow)
{
	Inventory = NewInventorySlots;
	SlotsPerRow = pSlotsPerRow;
	UIItemSlots.Reserve(pSlotsPerRow);
	for (int i = 0; i < pSlotsPerRow && i < NewInventorySlots->GetNumSlots(); i++)
	{

		ItemSlotCombo comb;
		comb.InvSlot = CreateWidget<UUI_ItemSlot>(GetWorld(), UUI_ItemSlot::StaticClass());
		comb.InvSlot->SetItemStack(NewInventorySlots->GetStackAt(i));
		comb.UISlot = Root->AddChildToCanvas(comb.InvSlot);
		comb.UISlot->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
		float angle = i * 360.f / pSlotsPerRow;
		angle = angle <= 90 ? (-angle + 90) : (-angle + 450);
		FVector2D Pos;
		Pos.X = sinf(angle * DEG_TO_RAD) * DistFromCenter;
		Pos.Y = cosf(angle * DEG_TO_RAD) * DistFromCenter;
		Pos.X -= 75.f;
		Pos.Y -= 75.f;
		comb.UISlot->SetPosition(Pos);
		UIItemSlots.Add(comb);
		if (!First)
			First = comb.InvSlot;
	}
}