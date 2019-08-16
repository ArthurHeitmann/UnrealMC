


#include "ItemSlots.h"
#include "../Items/Item.h"
#include "../MC_FakeGameModeBase.h"
#include "Engine/World.h"
#include "../UI/ItemSlotsHUD.h"

UItemSlots::UItemSlots()
{
	SetNumSlots(1);
}

FItemStack UItemSlots::GetStackAt(int x)
{
	if (x >= Slots.Num() || x < 0)
		throw - 1;

	return Slots[x];
}

void UItemSlots::SetNumSlots(int Num)
{
	Slots.SetNum(Num);
}

FItemStack UItemSlots::PickupItemStack(FItemStack Items)
{
	for (int i = 0; i < Slots.Num() && Items.ItemCount > 0; i++)
	{
		FItemStack& CurrentStack = Slots[i];
		if (CurrentStack.ItemS == nullptr ||
			(CurrentStack.ItemS->IsItemStackable() && CurrentStack.ItemCount < CurrentStack.ItemS->GetMaxStackCount() && CurrentStack.ItemS->IsStackableWith(Items.ItemS)))
		{
			if (CurrentStack.ItemS == nullptr)
				CurrentStack.ItemS = Items.ItemS;
			if (CurrentStack.ItemCount + Items.ItemCount > CurrentStack.ItemS->GetMaxStackCount())
			{
				int32 overflow = CurrentStack.ItemS->GetMaxStackCount() - CurrentStack.ItemCount;
				CurrentStack.ItemCount = CurrentStack.ItemS->GetMaxStackCount();
				Items.ItemCount -= overflow;
			}
			else
			{
				CurrentStack.ItemCount += Items.ItemCount;
				Items.ItemCount = 0;
			}
		}
	}
	return Items;
}

void UItemSlots::DebugPrint(UWorld* world)
{
	//UE_LOG(LogTemp, Warning, TEXT("Inventory:"));
	AMC_FakeGameModeBase* GM = Cast<AMC_FakeGameModeBase>(world->GetAuthGameMode());
	for (int i = 0; i < Slots.Num(); i++)
	{
		GM->ItemSlotWidget->SetSlotItemStack(i, Slots[i]);
		/*if (Slots[i].ItemS)
		{
			UE_LOG(LogTemp, Warning, TEXT("%d: %d x %d"), i, Slots[i].ItemCount, ( int) Slots[i].ItemS->GetItemEnum());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("%d: 0"), i);
		}*/
	}
}

UItemSlots::~UItemSlots()
{
	UE_LOG(LogTemp, Warning, TEXT("Destroying ItemSlots!!!!!!!!!!!!!"));
}
