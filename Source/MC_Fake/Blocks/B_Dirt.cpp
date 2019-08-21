#include "B_Dirt.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Texture2D.h"
#include "../World/McWorld.h"
#include "Enums.h"

//UMaterialInstanceDynamic* B_Dirt::DirtMaterial;

B_Dirt::B_Dirt()
{
	BlockID = 2;
	BlockName = TEXT("Dirt");
	BlockModelType = EBlockModelType::BLOCK;
	BlockEnum = BDirt;
	BreakingAction = ABreakSand;
	static ConstructorHelpers::FObjectFinder<UTexture> TextureObj(TEXT("Texture2D'/Game/Materials/Textures/dirt.dirt'"));
	Texture = TextureObj.Object;
}

UMaterialInstanceDynamic* B_Dirt::GetMaterial(UObject* UObj)
{
	UMaterialInstanceDynamic* OutMat = UMaterialInstanceDynamic::Create(BlockMaterial, UObj);
	OutMat->SetTextureParameterValue(TEXT("Block Texture"), Texture);
	return OutMat;
}

Block* B_Dirt::Clone()
{
	return new B_Dirt();
}
