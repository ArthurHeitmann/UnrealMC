


#include "UI_QuickAccessSlots.h"
#include "ConstructorHelpers.h"
#include "../Player/Inventory/ItemSlots.h"
#include "CanvasPanel.h"
#include "CanvasPanelSlot.h"
#include "WidgetTree.h"
#include "TextBlock.h"

const float UUI_QuickAccessSlots::DEG_TO_RAD = 0.0174532925199;

UUI_QuickAccessSlots::UUI_QuickAccessSlots(const FObjectInitializer& FOs) : Super(FOs)
{
	//Super::NativeConstruct();

	//static ConstructorHelpers::FObjectFinder<UCurveFloat> CurveFinder(TEXT("CurveFloat'/Game/Animation/Curves/C_ItemWheelToggle.C_ItemWheelToggle'"));
	//TimeCurve = CurveFinder.Object;

	//InterpFunction.BindUFunction(this, TEXT("TLUpdate"));
}

/*void UUI_QuickAccessSlots::SetSelecteddItemPointer(Item** NewItemPointer)
{
	SelectedItem = NewItemPointer;
}*/

bool UUI_QuickAccessSlots::Initialize()
{
	if (Super::Initialize() == false)
		return false;

	SelectedItemId = 0;
	SelectedRowID = 0;
	DistFromCenter = 350;

	Root = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("QuAcSl Root Canvas"));
	WidgetTree->RootWidget = Root;
	
	bIsVisible = false;
	SetVisibility(ESlateVisibility::Hidden);
	//SetRenderOpacity(0);

	//TLComp = NewObject<UTimelineComponent>(this, TEXT());
	//TLComp->CreationMethod = EComponentCreationMethod::UserConstructionScript;   //TODO Try removing this
	//TLComp->AddInterpFloat(TimeCurve, InterpFunction, TEXT("TL Float"));

	return true;
}

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
	//if (!TLComp->IsRegistered())
		//TLComp->RegisterComponent();

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
	}
}

void UUI_QuickAccessSlots::SetSelectedItemPointer(FItemStack const** ISP)
{
	SelectedItem = ISP;
	SelectItemStack(SelectedItemId);
}

void UUI_QuickAccessSlots::SelectItemStack(int32 id)
{
	*SelectedItem = Inventory->GetStackAt(SlotsPerRow * SelectedRowID + id);
}

void UUI_QuickAccessSlots::SelectNextItem()
{
	SelectedItemId = ++SelectedItemId % SlotsPerRow;
	SelectItemStack(SelectedItemId);
}

void UUI_QuickAccessSlots::SelectPreviousItem()
{
	SelectedItemId = --SelectedItemId % SlotsPerRow;
	if (SelectedItemId < 0)
		SelectedItemId += SlotsPerRow;
	SelectItemStack(SelectedItemId);
}

void UUI_QuickAccessSlots::ToggleVisibility()
{
	/*if (!TLComp)
	{
		TLComp = NewObject<UTimelineComponent>(GetWorld(), TEXT("Item Wheel Toggle Timeline"));
		TLComp->RegisterComponent();
		TLComp->CreationMethod = EComponentCreationMethod::UserConstructionScript;
		TLComp->SetLooping(false);
		FOnTimelineFloat onTimelineCallback;
		FOnTimelineEventStatic FinishedCallback;
		onTimelineCallback.BindUFunction(this, TEXT("TLUpdate"));
		FinishedCallback.BindUFunction(this, TEXT("TLFinished"));
		TLComp->AddInterpFloat(TimeCurve, onTimelineCallback);
		TLComp->SetTimelineFinishedFunc(FinishedCallback);
	}

	if (TLComp->IsPlaying())
		TLComp->Stop();*/

	if (bIsVisible)
		SetVisibility(ESlateVisibility::Hidden);
	else
		SetVisibility(ESlateVisibility::Visible);
	bIsVisible = !bIsVisible;
		
	//TLComp->PlayFromStart();
}

//
//void UUI_QuickAccessSlots::TLUpdate(float val)
//{
//	SetRenderOpacity(bIsVisible ? 1 - val : val);
//}
//
//void UUI_QuickAccessSlots::TLFinished()
//{
//	if (bIsVisible)
//		SetVisibility(ESlateVisibility::Hidden);
//	bIsVisible = !bIsVisible;
//}
