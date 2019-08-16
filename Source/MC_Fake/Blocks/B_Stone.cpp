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
	BreakTime = 1.5f;
	//BreakTime = 7.5f;
	static ConstructorHelpers::FObjectFinder<UTexture> TextureObj(TEXT("Texture2D'/Game/Materials/Textures/stone.stone'"));
	Texture = TextureObj.Object;
}

UMaterialInstanceDynamic* B_Stone::GetMaterial(UObject* UObj)
{
	UMaterialInstanceDynamic* OutMat = UMaterialInstanceDynamic::Create(Block::BlockMaterial, UObj);
	OutMat->SetTextureParameterValue(TEXT("Block Texture"), Texture);
	return OutMat;
}
