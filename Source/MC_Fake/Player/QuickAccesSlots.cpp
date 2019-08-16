


#include "QuickAccesSlots.h"

UQuickAccesSlots::UQuickAccesSlots()
{
	SetNumSlots(10);
	SelectedItemId = 0;
}

void UQuickAccesSlots::SetSelecteddItemPointer(Item** NewItemPointer)
{
	SelectedItem = NewItemPointer;
}

void UQuickAccesSlots::SetSelecteddItemSlotById(int32 id)
{
	if (id < Slots.Num() && id >= 0)
		SelectedItemId = id;
	else if (id >= 0)
		SelectedItemId = id % Slots.Num();
	else
		SelectedItemId = -id % Slots.Num();

	*SelectedItem = Slots[SelectedItemId].ItemS;
}

void UQuickAccesSlots::NextSelectedItem()
{
	SelectedItemId = ++SelectedItemId % Slots.Num();
	*SelectedItem = Slots[SelectedItemId].ItemS;
}

void UQuickAccesSlots::PreviousSelectedItem()
{
	SelectedItemId = --SelectedItemId % Slots.Num();
	*SelectedItem = Slots[SelectedItemId].ItemS;
}
