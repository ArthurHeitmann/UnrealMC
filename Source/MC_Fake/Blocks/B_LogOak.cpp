


#include "B_LogOak.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

B_LogOak::B_LogOak()
{
	BlockID = 17;
	SubID = 0;
	BlockName = TEXT("Log Oak");
	BlockModelType = BLOCK;
	BreakingAction = AWood;
	BlockEnum = BLog_Oak;
	BreakTime = 3;
	static ConstructorHelpers::FObjectFinder<UTexture> TextureObj(TEXT("Texture2D'/Game/Materials/Textures/log_oak.log_oak'"));
	//static ConstructorHelpers::FObjectFinder<UTexture> TextureBmpObj(TEXT("Texture2D'/Game/Materials/Textures/log_oak_BMP.log_oak_BMP'"));
	Texture = TextureObj.Object;
	//TextureBMP = TextureBmpObj.Object;
}

TArray<FVector2D> B_LogOak::GetTopUVs()
{
	return {
		{0.5, 0},
		{1, 0},
		{1, 1},
		{0.5, 1}
	};
}

TArray<FVector2D> B_LogOak::GetFrontUVs()
{
	return {
		{0, 0},
		{0.5, 0},
		{0.5, 1},
		{0, 1}
	};
}

TArray<FVector2D> B_LogOak::GetRightUVs()
{
	return {
		{0, 0},
		{0.5, 0},
		{0.5, 1},
		{0, 1}
	};
}

TArray<FVector2D> B_LogOak::GetBottomUVs()
{
	return {
		{0.5, 1},
		{0.5, 0},
		{1, 0},
		{1, 1}
	};
}

TArray<FVector2D> B_LogOak::GetLeftUVs()
{
	return {
		{0.5, 0},
		{0.5, 1},
		{0, 1},
		{0, 0}
	};
}

TArray<FVector2D> B_LogOak::GetBackUVs()
{
	return {
		{0, 0},
		{0.5, 0},
		{0.5, 1},
		{0, 1}
	};
}

TArray<FVector2D> B_LogOak::GetAllUVs()
{
	return {
		{0.5, 0},
		{1, 0},
		{1, 1},
		{0.5, 1},
		{0, 0},
		{0.5, 0},
		{0.5, 1},
		{0, 1},
		{0.5, 1},
		{0.5, 0},
		{1, 0},
		{1, 1},
		{0.5, 0},
		{0.5, 1},
		{0, 1},
		{0, 0},
		{0, 0},
		{0.5, 0},
		{0.5, 1},
		{0, 1},
		{0, 0},
		{0.5, 0},
		{0.5, 1},
		{0, 1}
	};
}

Block* B_LogOak::Clone()
{
	return new B_LogOak();
}
