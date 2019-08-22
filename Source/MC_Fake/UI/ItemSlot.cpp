

#include "ItemSlot.h"
#include "WidgetTree.h"
#include "TextBlock.h"
#include "Overlay.h"
#include "OverlaySlot.h"

void UItemSlot::NativeConstruct()
{
	Super::NativeConstruct();

}

bool UItemSlot::Initialize()
{
	if (Super::Initialize() == false)
		return false;

	UOverlay* Root = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), FName("Root"));
	WidgetTree->RootWidget = Root;
	NameText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Item Name Text"));
	NumberText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Item Number Text"));
	UOverlaySlot* NameSlot = Root->AddChildToOverlay(NameText);
	UOverlaySlot* NumberSlot = Root->AddChildToOverlay(NumberText);
	NameSlot->SetHorizontalAlignment(HAlign_Left);
	NameSlot->SetVerticalAlignment(VAlign_Top);
	NumberSlot->SetHorizontalAlignment(HAlign_Left);
	NumberSlot->SetVerticalAlignment(VAlign_Bottom);
	NumberSlot->SetPadding(FMargin(100, 50, 0, 0));
	return true;
}

void UItemSlot::SetItemStack(const FItemStack& NewItemStack)
{
	if (NewItemStack.ItemS)
		NameText->SetText(FText::FromName(NewItemStack.ItemS->GetName()));
	else
		NameText->SetText(FText::FromString("No Item"));
	if (NewItemStack.ItemCount)
		NumberText->SetText(FText::AsNumber(NewItemStack.ItemCount));
	else
		NumberText->SetText(FText::AsNumber(0));
	ItemStackC = NewItemStack;
}
