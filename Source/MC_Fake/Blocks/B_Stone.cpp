#include "B_Stone.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Texture2D.h"
#include "Enums.h"


B_Stone::B_Stone()
{
	BlockID = 1;
	BlockName = TEXT("Stone");
	BlockModelType = EBlockModelType::BLOCK;
	BlockEnum = BStone;
	BreakingAction = APickaxe;
	BreakTime = 7.5f;
	static ConstructorHelpers::FObjectFinder<UTexture> TextureObj(TEXT("Texture2D'/Game/Materials/Textures/stone.stone'"));
	Texture = TextureObj.Object;
}

B_Block* B_Stone::Clone()
{
	return new B_Stone();
}


//BlockRegistryImplementation(B_Stone)