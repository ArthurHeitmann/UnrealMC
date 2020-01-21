#include "B_Leaves.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

UMaterial* B_Leaves::WavyMaterialBase;

B_Leaves::B_Leaves()
{
	BlockID = 18;
	BlockModelType = BLOCK;
	BreakingAction = ABreakPlant;
	BreakTime = 0.35;

	if (!WavyMaterialBase)
	{
		static ConstructorHelpers::FObjectFinder<UMaterial> WavyMatFinder(TEXT("Material'/Game/Materials/Blocks/M_WavyBlock.M_WavyBlock'"));
		WavyMaterialBase = WavyMatFinder.Object;
	}
}

bool B_Leaves::IsBlockOpaque()
{
	return false;
}

void B_Leaves::OnBreak(UWorld* World, FVector Location)
{
}

UMaterialInstanceDynamic* B_Leaves::GetMaterial(UObject* UObj)
{
	auto outMat = UMaterialInstanceDynamic::Create(WavyMaterialBase, UObj);
	outMat->SetTextureParameterValue("Block Texture", Texture);
	return outMat;
}

B_Block* B_Leaves::Clone()
{
	return new B_Leaves;
}

