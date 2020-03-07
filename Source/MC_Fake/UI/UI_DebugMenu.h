#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI_DebugMenu.generated.h"

#define DefineDebugInput(Name) UPROPERTY() \
	class UEditableTextBox* Name##Input;

#define CreateDebugInput(Name) \
	UHorizontalBox* Name##Box = WidgetTree->ConstructWidget<UHorizontalBox>();\
	Name##Input = WidgetTree->ConstructWidget<UEditableTextBox>();\
	UTextBlock* Name##Text = WidgetTree->ConstructWidget<UTextBlock>(); \
	Name##Text->SetText(FText::FromString(#Name));\
	Name##Box->AddChildToHorizontalBox(Name##Text); \
	UHorizontalBoxSlot* Name##InputSlot = Name##Box->AddChildToHorizontalBox(Name##Input); \
	Name##InputSlot->SetSize(FSlateChildSize(ESlateSizeRule::Type::Fill));\
	Name##InputSlot->SetPadding(FMargin(25, 0, 0, 0));\
	UVerticalBoxSlot* Name##SettingsSlot = SettingsList->AddChildToVerticalBox(Name##Box); \
	\
	Name##Input->OnTextCommitted.AddDynamic(this, &UI_DebugMenu::Update##Name);

/**
 * 
 */
UCLASS()
class MC_FAKE_API UI_DebugMenu : public UUserWidget
{
	
	GENERATED_BODY()

	void (UObject::*BackFunction)(UWidget*);
	UPROPERTY()
	UObject* BackFuncUO;
	UPROPERTY()
	class UCanvasPanel* Root;
	UPROPERTY()
	class UVerticalBox* SettingsList;
	UPROPERTY()
	class UButton* BackBtn;
	DefineDebugInput(StageDirtGrass)
	DefineDebugInput(StageCaveCarving)
	DefineDebugInput(StageTrees)
	DefineDebugInput(TerrainHeightMultiplier)
	DefineDebugInput(TerrainHeight)
	DefineDebugInput(TurbulenceMult)
	DefineDebugInput(HeightOctaves)
	DefineDebugInput(HeightFrequency)
	DefineDebugInput(TurbulenceOctaves)
	DefineDebugInput(TurbulenceFrequency)
	DefineDebugInput(SlopeGrass)
	DefineDebugInput(SlopeDirt)
	DefineDebugInput(CaveOctaves)
	DefineDebugInput(CaveFrequency)
	DefineDebugInput(CaveThreshold)
	DefineDebugInput(SeaLevel)

	bool Initialize() override;
	UFUNCTION()
	void ResetWorldBtn();
	UFUNCTION()
	void InternalBackBtnBackCall();

	UFUNCTION()
	void UpdateTerrainHeight(const FText& Text, ETextCommit::Type CommitMethod);
	UFUNCTION()
	void UpdateStageDirtGrass(const FText& Text, ETextCommit::Type CommitMethod);
	UFUNCTION()
	void UpdateStageCaveCarving(const FText& Text, ETextCommit::Type CommitMethod);
	UFUNCTION()
	void UpdateStageTrees(const FText& Text, ETextCommit::Type CommitMethod);
	UFUNCTION()
	void UpdateTerrainHeightMultiplier(const FText& Text, ETextCommit::Type CommitMethod);
	UFUNCTION()
	void UpdateTurbulenceMult(const FText& Text, ETextCommit::Type CommitMethod);
	UFUNCTION()
	void UpdateHeightOctaves(const FText& Text, ETextCommit::Type CommitMethod);
	UFUNCTION()
	void UpdateHeightFrequency(const FText& Text, ETextCommit::Type CommitMethod);
	UFUNCTION()
	void UpdateTurbulenceOctaves(const FText& Text, ETextCommit::Type CommitMethod);
	UFUNCTION()
	void UpdateTurbulenceFrequency(const FText& Text, ETextCommit::Type CommitMethod);
	UFUNCTION()
	void UpdateSlopeGrass(const FText& Text, ETextCommit::Type CommitMethod);
	UFUNCTION()
	void UpdateSlopeDirt(const FText& Text, ETextCommit::Type CommitMethod);
	UFUNCTION()
	void UpdateCaveOctaves(const FText& Text, ETextCommit::Type CommitMethod);
	UFUNCTION()
	void UpdateCaveFrequency(const FText& Text, ETextCommit::Type CommitMethod);
	UFUNCTION()
	void UpdateCaveThreshold(const FText& Text, ETextCommit::Type CommitMethod);
	UFUNCTION()
	void UpdateSeaLevel(const FText& Text, ETextCommit::Type CommitMethod);
public:
	void SetBackFunction(UObject* FuncUO, void(UObject::*BackFunc)(UWidget*));
};
