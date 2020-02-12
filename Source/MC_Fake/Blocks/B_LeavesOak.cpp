


#include "B_LeavesOak.h"
#include "ConstructorHelpers.h"

B_LeavesOak::B_LeavesOak()
{
	SubID = 0;
	BlockName = TEXT("Leaves Oak");
	static ConstructorHelpers::FObjectFinder<UTexture> TextureObj(TEXT("Texture2D'/Game/Materials/Textures/Leaves_Oak.Leaves_Oak'"));
	Texture = TextureObj.Object;
}
