#include "B_Water.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

B_Water::B_Water()
{
	BlockID = 9;
	BlockName = TEXT("Normal Water");
	BlockModelType = WATER;
	BlockEnum = BWater;
	BreakTime = 999999999;
	static ConstructorHelpers::FObjectFinder<UTexture> TextureObj(TEXT("Texture2D'/Game/Materials/Textures/water_still.water_still'"));
	Texture = TextureObj.Object;
}

UMaterialInstanceDynamic* B_Water::GetMaterial(UObject* UObj)
{
	UMaterialInstanceDynamic* OutMat = UMaterialInstanceDynamic::Create(Block::BlockMaterial, UObj);
	OutMat->SetTextureParameterValue(TEXT("Block Texture"), Texture);
	return OutMat;
}

bool B_Water::IsBlockOpaque()
{
	return false;
}
