#include "B_LogOak.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

B_LogOak::B_LogOak()
{
	SubID = 0;
	BlockName = TEXT("Log Oak");
	BlockEnum = BLog_Oak;
	static ConstructorHelpers::FObjectFinder<UTexture> TextureObj(TEXT("Texture2D'/Game/Materials/Textures/log_oak.log_oak'"));
	Texture = TextureObj.Object;
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

B_Block* B_LogOak::Clone()
{
	return new B_LogOak();
}
