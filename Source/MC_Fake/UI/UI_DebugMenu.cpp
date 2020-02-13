#include "UI_DebugMenu.h"
#include "WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"

bool UI_DebugMenu::Initialize()
{
	if (Super::Initialize() == false)
		return false;

	Root = WidgetTree->ConstructWidget<UCanvasPanel>();
	SettingsList = WidgetTree->ConstructWidget<UVerticalBox>();
	BackBtn = WidgetTree->ConstructWidget<UButton>();
	UTextBlock* BackBtnText = WidgetTree->ConstructWidget<UTextBlock>();

	WidgetTree->RootWidget = Root;
	Root->AddChildToCanvas(SettingsList);
	SettingsList->AddChildToVerticalBox(BackBtn);
	BackBtn->AddChild(BackBtnText);

	BackBtnText->SetText(FText::FromString("<--"));
	BackBtn->OnClicked.AddDynamic(this, &UI_DebugMenu::InternalBackBtnBackCall);

	return true;
}

void UI_DebugMenu::InternalBackBtnBackCall()
{
	if (BackFunction)
		(BackFuncUO->*(BackFunction))(this);
}

void UI_DebugMenu::SetBackFunction(UObject* FuncUO, void(UObject::*BackFunc)(UWidget*))
{
	BackFuncUO = FuncUO;
	BackFunction = BackFunc;
}
