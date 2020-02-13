

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI_PauseMenu.generated.h"

/**
 * 
 */
UCLASS()
class MC_FAKE_API UI_PauseMenu : public UUserWidget
{
	GENERATED_BODY()

protected:
	bool Initialize() override;
	class UCanvasPanel* Root;
	class UVerticalBox* MenuList;
	class UI_DebugMenu* DebugMenu;

	void ResetView(UWidget* ChildWidget);

	UFUNCTION()
	void DebugMenuClick();
};
