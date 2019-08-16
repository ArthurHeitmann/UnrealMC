// Fill out your copyright notice in the Description page of Project Settings.


#include "MC_FakeGameModeBase.h"
#include "UI/ItemSlotsHUD.h"

void AMC_FakeGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	ItemSlotWidget = CreateWidget<UItemSlotsHUD>(GetWorld());
	ItemSlotWidget->SetSize(10);
	ItemSlotWidget->AddToViewport();
}