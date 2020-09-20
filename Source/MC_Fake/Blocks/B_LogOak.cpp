#include "B_LogOak.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

B_LogOak::B_LogOak()
{
	SubID = 0;
	BlockName = TEXT("Log Oak");
	static ConstructorHelpers::FObjectFinder<UTexture> TextureObj(TEXT("Texture2D'/Game/Materials/Textures/Log_Oak.Log_Oak'"));
	Texture = TextureObj.Object;
}

B_Block* B_LogOak::Clone()
{
	return new B_LogOak();
}
