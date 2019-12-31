// Fill out your copyright notice in the Description page of Project Settings.


#include "B_Stone.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Texture2D.h"
#include "../World/McWorld.h"
#include "Enums.h"

//static UTexture* StoneTexture;

B_Stone::B_Stone()
{
	BlockID = 1;
	BlockName = TEXT("Stone");
	BlockModelType = EBlockModelType::BLOCK;
	BlockEnum = BStone;
	BreakingAction = APickaxe;
	BreakTime = 7.5f;
	static ConstructorHelpers::FObjectFinder<UTexture> TextureObj(TEXT("Texture2D'/Game/Materials/Textures/stone.stone'"));
	//static ConstructorHelpers::FObjectFinder<UTexture> TextureBmpObj(TEXT("Texture2D'/Game/Materials/Textures/stone_BMP.stone_BMP'"));
	Texture = TextureObj.Object;
	//TextureBMP = TextureBmpObj.Object;
}

B_Block* B_Stone::Clone()
{
	return new B_Stone();
}
