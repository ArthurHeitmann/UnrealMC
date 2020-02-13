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
	DebugMenu = CreateWidget<UI_DebugMenu>(GetWorld(), UI_DebugMenu::StaticClass());
	//DebugMenu = WidgetTree->ConstructWidget<UI_DebugMenu>(UI_DebugMenu::StaticClass());
	
	DebugMenuBtn->OnClicked.AddDynamic(this, &UI_PauseMenu::DebugMenuClick);
	DebugMenuText->SetText(FText::FromString("Debug Menu"));
	DebugMenu->SetBackFunction(this, static_cast<void(UObject::*)(UWidget*)>(&UI_PauseMenu::ResetView));

	DebugMenuBtn->AddChild(DebugMenuText);
	MenuList->AddChildToVerticalBox(DebugMenuBtn);
	Root->AddChildToCanvas(MenuList);

	return true;
}

void UI_PauseMenu::ResetView(UWidget* ChildWidget)
{
	Root->RemoveChild(ChildWidget);
	Root->AddChildToCanvas(MenuList);
}

void UI_PauseMenu::DebugMenuClick()
{
	Root->RemoveChild(MenuList);
	Root->AddChildToCanvas(DebugMenu);
}
