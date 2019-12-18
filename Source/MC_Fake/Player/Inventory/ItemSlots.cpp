


#include "ItemSlots.h"
#include "../../Items/Item.h"
#include "../../MC_FakeGameModeBase.h"
#include "Engine/World.h"
#include "../../UI/ItemSlotsHUD.h"

UItemSlots::UItemSlots()
{
	//SetNumSlots(1);
}

FItemStack* UItemSlots::GetStackAt(int x)
{
	if (x >= Slots.Num() || x < 0)
		throw - 1;

	return Slots[x];
}

void UItemSlots::SetNumSlots(int Num)
{
	for (FItemStack* IS : Slots)
	{
		if (IS && IS->ItemS)
			delete IS->ItemS;
	}
	Slots.SetNum(Num);
	for (FItemStack*& IS : Slots)
	{
		IS = new FItemStack(new I_NoItem(), 0);
	}
}

int32 UItemSlots::GetNumSlots()
{
	return Slots.Num();
}

FItemStack UItemSlots::PickupItemStack(FItemStack Items)
{
	TArray<FItemStack*> OrderedEmptySlots;
	TArray<FItemStack*> OrderedSameTypeSlots;

	for (int i = 0; i < Slots.Num() && Items.ItemCount > 0; ++i)
	{
		FItemStack* CurrentStack = Slots[i];

		if (CurrentStack->ItemS->GetItemEnum() == INoItem)
			OrderedEmptySlots.Add(CurrentStack);
		else if (CurrentStack->ItemS->IsItemStackable() &&
			CurrentStack->ItemCount < CurrentStack->ItemS->GetMaxStackCount() &&
			CurrentStack->ItemS->IsStackableWith(Items.ItemS))
			OrderedSameTypeSlots.Add(CurrentStack);

		/*if (CurrentStack->ItemS->GetItemEnum() == INoItem ||
			()
		{
			if (CurrentStack->ItemS->GetItemEnum() == INoItem)
			{
				delete CurrentStack->ItemS;
				CurrentStack->ItemS = Items.ItemS;
			}
			if (CurrentStack->ItemCount + Items.ItemCount > CurrentStack->ItemS->GetMaxStackCount())
			{
				int32 overflow = CurrentStack->ItemS->GetMaxStackCount() - CurrentStack->ItemCount;
				CurrentStack->ItemCount = CurrentStack->ItemS->GetMaxStackCount();
				Items.ItemCount -= overflow;
			}
			else
			{
				CurrentStack->ItemCount += Items.ItemCount;
				Items.ItemCount = 0;
			}
		}*/
	}

	for (FItemStack*& SameSlot : OrderedSameTypeSlots)
	{
		if (Items.ItemCount == 0)
			break;

		if (SameSlot->ItemCount + Items.ItemCount > SameSlot->ItemS->GetMaxStackCount())
		{
			int32 overflow = SameSlot->ItemS->GetMaxStackCount() - SameSlot->ItemCount;
			SameSlot->ItemCount = SameSlot->ItemS->GetMaxStackCount();
			Items.ItemCount -= overflow;
		}
		else
		{
			SameSlot->ItemCount += Items.ItemCount;
			Items.ItemCount = 0;
		}
	}
	for (FItemStack*& EmptySlot : OrderedEmptySlots)
	{
		if (Items.ItemCount == 0)
			break;

		delete EmptySlot->ItemS;
		EmptySlot->ItemS = Items.ItemS;

		if (EmptySlot->ItemCount + Items.ItemCount > EmptySlot->ItemS->GetMaxStackCount())
		{
			int32 overflow = EmptySlot->ItemS->GetMaxStackCount() - EmptySlot->ItemCount;
			EmptySlot->ItemCount = EmptySlot->ItemS->GetMaxStackCount();
			Items.ItemCount -= overflow;
		}
		else
		{
			EmptySlot->ItemCount += Items.ItemCount;
			Items.ItemCount = 0;
		}
	}

	return Items;
}

UItemSlots::~UItemSlots()
{
	for (int i = 0; i < Slots.Num(); ++i)
	{
		delete Slots[i]->ItemS;
		delete Slots[i];
	}

}
