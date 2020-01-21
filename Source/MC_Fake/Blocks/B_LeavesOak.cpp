


#include "B_LeavesOak.h"
#include "ConstructorHelpers.h"

B_LeavesOak::B_LeavesOak()
{
	SubID = 0;
	BlockName = TEXT("Leaves Oak");
	BlockEnum = BLeaves_Oak;
	static ConstructorHelpers::FObjectFinder<UTexture> TextureObj(TEXT("Texture2D'/Game/Materials/Textures/leaves_oak.leaves_oak'"));
	Texture = TextureObj.Object;
}
