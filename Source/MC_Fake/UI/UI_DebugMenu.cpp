#include "UI_DebugMenu.h"
#include "WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/EditableTextBox.h"
#include "SlateWidgetStyleAsset.h"
#include "World/ChunkGenerator.h"
#include "Kismet/GameplayStatics.h"
#include "DebugQuickCharacter.h"
#include "World/WorldLoadingComponent.h"


bool UI_DebugMenu::Initialize()
{
	if (Super::Initialize() == false)
		return false;

	Root = WidgetTree->ConstructWidget<UCanvasPanel>();
	SettingsList = WidgetTree->ConstructWidget<UVerticalBox>();
	BackBtn = WidgetTree->ConstructWidget<UButton>();
	UTextBlock* BackBtnText = WidgetTree->ConstructWidget<UTextBlock>();
	TerrainHeightInput = WidgetTree->ConstructWidget<UEditableTextBox>();
	UButton* ResetBtn = WidgetTree->ConstructWidget<UButton>();
	UTextBlock* ResetBtnText = WidgetTree->ConstructWidget<UTextBlock>();
	
	WidgetTree->RootWidget = Root;
	UCanvasPanelSlot* RootSlot = Root->AddChildToCanvas(SettingsList);
	SettingsList->AddChildToVerticalBox(BackBtn);
	BackBtn->AddChild(BackBtnText);
	ResetBtn->AddChild(ResetBtnText);
	SettingsList->AddChildToVerticalBox(ResetBtn);

	RootSlot->SetSize({ 400, 600 });
	BackBtnText->SetText(FText::FromString("<--"));
	BackBtn->OnClicked.AddDynamic(this, &UI_DebugMenu::InternalBackBtnBackCall);
	ResetBtnText->SetText(FText::FromString("Restart World"));
	ResetBtn->OnClicked.AddDynamic(this, &UI_DebugMenu::ResetWorldBtn);

	CreateDebugInput(StageDirtGrass)
	CreateDebugInput(StageCaveCarving)
	CreateDebugInput(StageTrees)
	CreateDebugInput(TerrainHeightMultiplier)
	CreateDebugInput(TerrainHeight)
	CreateDebugInput(HeightOctaves)
	CreateDebugInput(HeightFrequency)
	CreateDebugInput(TurbulenceMult)
	CreateDebugInput(TurbulenceOctaves)
	CreateDebugInput(TurbulenceFrequency)
	CreateDebugInput(SlopeGrass)
	CreateDebugInput(SlopeDirt)
	CreateDebugInput(CaveOctaves)
	CreateDebugInput(CaveFrequency)
	CreateDebugInput(CaveThreshold)
	CreateDebugInput(SeaLevel)
	

	return true;
}

void UI_DebugMenu::ResetWorldBtn()
{
	ADebugQuickCharacter* ch = Cast<ADebugQuickCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (ch)
		ch->GetWorldLoadingComponent()->ResetWorld();
}

void UI_DebugMenu::SetBackFunction(UObject* FuncUO, void(UObject::* BackFunc)(UWidget*))
{
	BackFuncUO = FuncUO;
	BackFunction = BackFunc;
}


void UI_DebugMenu::InternalBackBtnBackCall()
{
	if (BackFunction)
		(BackFuncUO->*(BackFunction))(this);
}

void UI_DebugMenu::UpdateTerrainHeight(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::Type::OnEnter && Text.IsNumeric())
		ChunkGenerator::BaseTerrainHeight = FCString::Atof(*Text.ToString());
}

void UI_DebugMenu::UpdateStageDirtGrass(const FText& Text, ETextCommit::Type CommitMethod)
{
	FString s = Text.ToString();
	if (CommitMethod == ETextCommit::Type::OnEnter)
		ChunkGenerator::bEnableStageDirtGrass = s.ToBool();
}

void UI_DebugMenu::UpdateStageCaveCarving(const FText& Text, ETextCommit::Type CommitMethod)
{
	FString s = Text.ToString();
	if (CommitMethod == ETextCommit::Type::OnEnter)
		ChunkGenerator::bEnableStageCaveCarving = s.ToBool();
}

void UI_DebugMenu::UpdateStageTrees(const FText& Text, ETextCommit::Type CommitMethod)
{
	FString s = Text.ToString();
	if (CommitMethod == ETextCommit::Type::OnEnter)
		ChunkGenerator::bEnableStageTrees = s.ToBool();
}

void UI_DebugMenu::UpdateTerrainHeightMultiplier(const FText& Text, ETextCommit::Type CommitMethod)
{
	FString s = Text.ToString();
	if (CommitMethod == ETextCommit::Type::OnEnter)
		ChunkGenerator::TerrainHeightMultiplier = FCString::Atof(*s);
}

void UI_DebugMenu::UpdateTurbulenceMult(const FText& Text, ETextCommit::Type CommitMethod)
{
	FString s = Text.ToString();
	if (CommitMethod == ETextCommit::Type::OnEnter)
		ChunkGenerator::TerrainTurbulenceMultiplier = FCString::Atof(*s);
}

void UI_DebugMenu::UpdateHeightOctaves(const FText& Text, ETextCommit::Type CommitMethod)
{
	FString s = Text.ToString();
	if (CommitMethod == ETextCommit::Type::OnEnter)
		ChunkGenerator::HeightMapOctaves = FCString::Atoi(*s);
}

void UI_DebugMenu::UpdateHeightFrequency(const FText& Text, ETextCommit::Type CommitMethod)
{
	FString s = Text.ToString();
	if (CommitMethod == ETextCommit::Type::OnEnter)
		ChunkGenerator::HeightMapFrequency = FCString::Atof(*s);
}

void UI_DebugMenu::UpdateTurbulenceOctaves(const FText& Text, ETextCommit::Type CommitMethod)
{
	FString s = Text.ToString();
	if (CommitMethod == ETextCommit::Type::OnEnter)
		ChunkGenerator::TurbulenceOctaves = FCString::Atoi(*s);
}

void UI_DebugMenu::UpdateTurbulenceFrequency(const FText& Text, ETextCommit::Type CommitMethod)
{
	FString s = Text.ToString();
	if (CommitMethod == ETextCommit::Type::OnEnter)
		ChunkGenerator::TurbulenceFrequency = FCString::Atof(*s);
}

void UI_DebugMenu::UpdateSlopeGrass(const FText& Text, ETextCommit::Type CommitMethod)
{
	FString s = Text.ToString();
	if (CommitMethod == ETextCommit::Type::OnEnter)
		ChunkGenerator::SlopeGrass = FCString::Atof(*s);
}

void UI_DebugMenu::UpdateSlopeDirt(const FText& Text, ETextCommit::Type CommitMethod)
{
	FString s = Text.ToString();
	if (CommitMethod == ETextCommit::Type::OnEnter)
		ChunkGenerator::SlopeDirt = FCString::Atof(*s);
}

void UI_DebugMenu::UpdateCaveOctaves(const FText& Text, ETextCommit::Type CommitMethod)
{
	FString s = Text.ToString();
	if (CommitMethod == ETextCommit::Type::OnEnter)
		ChunkGenerator::CaveOctaves = FCString::Atoi(*s);
}

void UI_DebugMenu::UpdateCaveFrequency(const FText& Text, ETextCommit::Type CommitMethod)
{
	FString s = Text.ToString();
	if (CommitMethod == ETextCommit::Type::OnEnter)
		ChunkGenerator::CaveFrequency = FCString::Atof(*s);
}

void UI_DebugMenu::UpdateCaveThreshold(const FText& Text, ETextCommit::Type CommitMethod)
{
	FString s = Text.ToString();
	if (CommitMethod == ETextCommit::Type::OnEnter)
		ChunkGenerator::CaveThreshold = FCString::Atof(*s);
}

void UI_DebugMenu::UpdateSeaLevel(const ::FText& Text, ETextCommit::Type CommitMethod)
{
	FString s = Text.ToString();
	if (CommitMethod == ETextCommit::Type::OnEnter)
		ChunkGenerator::SeaLevel = FCString::Atof(*s);
}