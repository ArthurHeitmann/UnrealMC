

#include "UI_ItemSlot.h"
#include "WidgetTree.h"
#include "TextBlock.h"
#include "Image.h"
#include "Overlay.h"
#include "OverlaySlot.h"
#include "CanvasPanel.h"
#include "CanvasPanelSlot.h"
#include "ItemRenderer.h"
#include "Border.h"
#include "PanelSlot.h"

bool UUI_ItemSlot::Initialize()
{
	if (Super::Initialize() == false)
		return false;

	UCanvasPanel* Root = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), FName("Root"));
	WidgetTree->RootWidget = Root;
	UOverlay* Root2 = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), FName("Second Root"));
	//
	UBorder* Root1_2 = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
	Root1_2->SetBrushColor(FLinearColor(0.f, 0.f, 0.f, 0.1f));
	UCanvasPanelSlot* CanvasSlot = Root->AddChildToCanvas(Root1_2);
	Root1_2->AddChild(Root2);
	//
	//UCanvasPanelSlot* CanvasSlot = Root->AddChildToCanvas(Root2);
	CanvasSlot->SetSize({ 150, 150 });
	Root2->SetClipping(EWidgetClipping::ClipToBounds);
	NameText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Item Name Text"));
	NumberText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Item Number Text"));
	RenderedImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("Rendered Item Preview"));
	UOverlaySlot* NameSlot = Root2->AddChildToOverlay(NameText);
	UOverlaySlot* NumberSlot = Root2->AddChildToOverlay(NumberText);
	UOverlaySlot* ImageSlot = Root2->AddChildToOverlay(RenderedImage);
	NameSlot->SetHorizontalAlignment(HAlign_Left);
	NameSlot->SetVerticalAlignment(VAlign_Top);
	NumberSlot->SetHorizontalAlignment(HAlign_Left);
	NumberSlot->SetVerticalAlignment(VAlign_Bottom);
	ImageSlot->SetHorizontalAlignment(HAlign_Fill);
	ImageSlot->SetVerticalAlignment(VAlign_Fill);
	
	return true;
}

void UUI_ItemSlot::NativeConstruct()
{
	Super::NativeConstruct();
}

void UUI_ItemSlot::SetItemStack(const FItemStack& NewItemStack)
{
	if (NewItemStack.ItemS)
		NameText->SetText(FText::FromName(NewItemStack.ItemS->GetName()));
	else
		NameText->SetText(FText::FromString(""));
	NumberText->SetText(FText::AsNumber(NewItemStack.ItemCount));

	if (ItemStackC.ItemS)
		delete ItemStackC.ItemS;
	ItemStackC = NewItemStack;

	if (!ItemRenderer)
	{
		ItemRenderer = GetWorld()->SpawnActor<AItemRenderer>();
	}
	ItemRenderer->InitWithItemStack(NewItemStack);
	TestMat = ItemRenderer->GetUIMaterial(this);
	RenderedImage->SetBrushFromMaterial(Cast<UMaterialInterface>(ItemRenderer->GetUIMaterial(this)));
}

UUI_ItemSlot::~UUI_ItemSlot()
{
	if (ItemRenderer && ItemRenderer->IsPendingKill())
		ItemRenderer->Destroy();
}
