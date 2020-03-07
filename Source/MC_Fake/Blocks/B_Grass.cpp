#include "B_Grass.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Texture2D.h"
#include "Enums.h"

B_Grass::B_Grass()
{
	BlockID = 3;
	BlockName = TEXT("Grass");
	BlockModelType = EBlockModelType::BLOCK;
	BreakingAction = ABreakPlant;
	BreakTime = 0.9f;
	static ConstructorHelpers::FObjectFinder<UTexture> TextureObj(TEXT("Texture2D'/Game/Materials/Textures/grass_flat.grass_flat'"));
	Texture = TextureObj.Object;
}

TArray<FVector2D> B_Grass::GetTopUVs()
{
	return {
		{0, -0.5},
		{.5, -0.5},
		{.5, -1},
		{0, -1}
	};
}

TArray<FVector2D> B_Grass::GetFrontUVs()
{
	return {
		{.5, 0},
		{1, 0},
		{1, -.5},
		{.5, -.5}
	};
}

TArray<FVector2D> B_Grass::GetRightUVs()
{
	return {
		{.5, 0},
		{1, 0},
		{1, -.5},
		{.5, -.5}
	};
}

TArray<FVector2D> B_Grass::GetBottomUVs()
{
	return {
		{0, -.5},
		{0, 0},
		{.5, 0},
		{.5, -.5}
	};
}

TArray<FVector2D> B_Grass::GetLeftUVs()
{
	return {
		{1, 0},
		{1, -.5},
		{0.5, -.5},
		{0.5, 0}
	};
}

TArray<FVector2D> B_Grass::GetBackUVs()
{
	return {
		{0.5, 0},
		{1, 0},
		{1, -.5},
		{0.5, -.5}
	};
}

TArray<FVector2D> B_Grass::GetAllUVs()
{
	return {
		{0, -0.5},
		{.5, -0.5},
		{.5, -1},
		{0, -1},
		{.5, 0},
		{1, 0},
		{1, -.5},
		{.5, -.5},
		{0, -.5},
		{0, 0},
		{.5, 0},
		{.5, -.5},
		{1, 0},
		{1, -.5},
		{0.5, -.5},
		{0.5, 0},
		{.5, 0},
		{1, 0},
		{1, -.5},
		{.5, -.5},
		{0.5, 0},
		{1, 0},
		{1, -.5},
		{0.5, -.5}
	};
}


B_Block* B_Grass::Clone()
{
	return new B_Grass();
}
