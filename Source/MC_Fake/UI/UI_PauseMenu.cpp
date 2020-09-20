#include "UI_PauseMenu.h"
#include "UI_DebugMenu.h"
#include "WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"


bool UI_PauseMenu::Initialize()
{
	if (Super::Initialize() == false)
		return false;
	
	Root = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
	WidgetTree->RootWidget = Root;
	MenuList = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	UButton* DebugMenuBtn = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
	UTextBlock* DebugMenuText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	
	DebugMenuBtn->OnClicked.AddDynamic(this, &UI_PauseMenu::DebugMenuClick);
	DebugMenuText->SetText(FText::FromString("Debug Menu"));

	DebugMenuBtn->AddChild(DebugMenuText);
	MenuList->AddChildToVerticalBox(DebugMenuBtn);
	Root->AddChildToCanvas(MenuList);

	return true;
}

void UI_PauseMenu::ResetView(UWidget* ChildWidget)
{
	DebugMenu->RemoveFromParent();
	this->AddToViewport();
}

void UI_PauseMenu::DebugMenuClick()
{
	this->RemoveFromParent();
	DebugMenu = CreateWidget<UI_DebugMenu>(GetWorld(), UI_DebugMenu::StaticClass());
	DebugMenu->SetBackFunction(this, static_cast<void(UObject::*)(UWidget*)>(&UI_PauseMenu::ResetView));
	DebugMenu->AddToViewport();
	DebugMenu->SetPositionInViewport({ 200, 100 });
}
