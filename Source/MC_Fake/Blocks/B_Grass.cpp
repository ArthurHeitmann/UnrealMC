


#include "B_Grass.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Texture2D.h"
#include "../World/McWorld.h"
#include "Enums.h"

//UMaterialInstanceDynamic* B_Grass::GrassBlockMaterial;

B_Grass::B_Grass()
{
	BlockID = 3;
	BlockName = TEXT("Grass");
	BlockModelType = EBlockModelType::BLOCK;
	BlockEnum = BGrass;
	BreakingAction = ABreakPlant;
	BreakTime = 0.9f;
	static ConstructorHelpers::FObjectFinder<UTexture> TextureObj(TEXT("Texture2D'/Game/Materials/Textures/grass.grass'"));
	//static ConstructorHelpers::FObjectFinder<UTexture> TextureBMPObj(TEXT("Texture2D'/Game/Materials/Textures/grass_texture_BMP.grass_texture_BMP'"));
	Texture = TextureObj.Object;
	//TextureBMP = TextureBMPObj.Object;
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


Block* B_Grass::Clone()
{
	return new B_Grass();
}
