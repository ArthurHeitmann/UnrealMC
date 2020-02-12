#include "B_Dirt.h"
#include "UObject/ConstructorHelpers.h"

B_Dirt::B_Dirt()
{
	BlockID = 2;
	BlockName = TEXT("Dirt");
	BlockModelType = EBlockModelType::BLOCK;
	BreakingAction = ABreakSand;
	static ConstructorHelpers::FObjectFinder<UTexture> TextureObj(TEXT("Texture2D'/Game/Materials/Textures/dirt.dirt'"));
	Texture = TextureObj.Object;
}

B_Block* B_Dirt::Clone()
{
	return new B_Dirt();
}

