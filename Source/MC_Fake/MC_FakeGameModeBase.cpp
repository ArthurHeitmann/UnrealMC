// Fill out your copyright notice in the Description page of Project Settings.


#include "MC_FakeGameModeBase.h"
#include "UI/ItemSlotsHUD.h"

void AMC_FakeGameModeBase::BeginPlay()
{
	Super::BeginPlay();
}

AMC_FakeGameModeBase::AMC_FakeGameModeBase()
{
	DefaultPawnClass = ADebugQuickCharacter::StaticClass();
}
