


#include "ItemSlotsHUD.h"
#include "WidgetTree.h"
#include "HorizontalBox.h"
#include "HorizontalBoxSlot.h"
#include "CanvasPanel.h"
#include "CanvasPanelSlot.h"

void UItemSlotsHUD::NativeConstruct()
{
	Super::NativeConstruct();
}

bool UItemSlotsHUD::Initialize()
{
	if (Super::Initialize() == false)
		return false;

	UCanvasPanel* Root = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("Root"));
	WidgetTree->RootWidget = Root;
	SlotContainer = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), FName("Item Slots Container"));
	ContentSlot = Root->AddChildToCanvas(SlotContainer);
	ContentSlot->SetMinimum({ .5, 1 });
	ContentSlot->SetMaximum({ .5, 1 });
	return true;
}

void UItemSlotsHUD::SetSize(int32 Count)
{
	SlotContainer->ClearChildren();
	ContentSlot->SetAutoSize(true);
	Slots.Empty();
	Slots.Reserve(Count);
	FVector2D screenSize;
	for (int i = 0; i < Count; ++i)
	{
		FString SlotName("Slot ");
		SlotName += FString::FromInt(i);
		UItemSlot* NewSlot = CreateWidget<UItemSlot>(GetWorld(), UItemSlot::StaticClass(), *SlotName);
		NewSlot->SetItemStack({});
		Slots.Add(NewSlot);
		UHorizontalBoxSlot* HSlot = SlotContainer->AddChildToHorizontalBox(NewSlot);
		HSlot->SetPadding(FMargin(10));
		FGeometry geom = NewSlot->GetCachedGeometry();
		FVector2D locSize = geom.GetLocalSize();
		FVector2D screenPos = geom.LocalToAbsolute(FVector2D(0, 0));
		screenSize = geom.LocalToAbsolute(locSize) - screenPos;
	}
	

	FVector2D Dimensions = ContentSlot->GetSize();
	//ContentSlot->SetAutoSize(false);
	ContentSlot->SetPosition({ -500, -150.f });
}

void UItemSlotsHUD::SetSlotItemStack(int32 SlotId, const FItemStack& NewItemStack)
{
	if (SlotId >= Slots.Num() || SlotId < 0)
	{
#ifdef UE_BUILD_DEBUG
		UE_LOG(LogTemp, Warning, TEXT("slot id %d is invalid"), SlotId);
#endif
		return;
	}

	Slots[SlotId]->SetItemStack(NewItemStack);
}
