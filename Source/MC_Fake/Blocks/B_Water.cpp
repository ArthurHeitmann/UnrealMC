#include "B_Water.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

B_Water::B_Water()
{
	BlockID = 9;
	BlockName = TEXT("Water");
	BreakTime = 999999999;
	static ConstructorHelpers::FObjectFinder<UTexture> TextureObj(TEXT("Texture2D'/Game/Materials/Textures/water_still.water_still'"));
	Texture = TextureObj.Object;
}

bool B_Water::IsBlockOpaque()
{
	return false;
}

B_Block* B_Water::Clone()
{
	return new B_Water();
}