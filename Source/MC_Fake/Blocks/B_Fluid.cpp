#include "B_Fluid.h"

#include "Materials/MaterialInstanceDynamic.h"
#include "ConstructorHelpers.h"

UMaterial* B_Fluid::FluidMaterial = nullptr; 

B_Fluid::B_Fluid()
{
	BlockModelType = FLUID;
	bHasCollision = false;
	static ConstructorHelpers::FObjectFinder<UMaterial> FluidMatFinder(TEXT("Material'/Game/Materials/Blocks/M_Fluid.M_Fluid'"));
	FluidMaterial = FluidMatFinder.Object;
}

bool B_Fluid::IsSideOptimizable(EDirection Direction)
{
	return false;
}

UMaterialInstanceDynamic* B_Fluid::GetMaterial(UObject* UObj)
{
	UMaterialInstanceDynamic* OutMat = UMaterialInstanceDynamic::Create(FluidMaterial, UObj);
	return OutMat;
}
