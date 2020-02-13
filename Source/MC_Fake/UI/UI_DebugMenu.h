

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI_DebugMenu.generated.h"

/**
 * 
 */
UCLASS()
class MC_FAKE_API UI_DebugMenu : public UUserWidget
{
	GENERATED_BODY()

	void (UObject::*BackFunction)(UWidget*);
	UObject* BackFuncUO;
	class UCanvasPanel* Root;
	class UVerticalBox* SettingsList;
	class UButton* BackBtn;

	bool Initialize() override;
	UFUNCTION()
	void InternalBackBtnBackCall();
public:
	void SetBackFunction(UObject* FuncUO, void(UObject::*BackFunc)(UWidget*));
};
