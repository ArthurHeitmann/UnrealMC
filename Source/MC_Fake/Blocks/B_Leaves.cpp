


#include "B_Leaves.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

B_Leaves::B_Leaves()
{
	BlockID = 18;
	SubID = 0;
	BlockName = TEXT("Leaves Oak");
	BlockModelType = BLOCK;
	BlockEnum = BLeaves_Oak;
	BreakingAction = ABreakPlant;
	BreakTime = 0.35;
	static ConstructorHelpers::FObjectFinder<UTexture> TextureObj(TEXT("Texture2D'/Game/Materials/Textures/leaves_oak.leaves_oak'"));
	Texture = TextureObj.Object;
}

bool B_Leaves::IsBlockOpaque()
{
	return false;
}

void B_Leaves::OnBreak(UWorld* World, FVector Location)
{
}

B_Block* B_Leaves::Clone()
{
	return new B_Leaves;
}

