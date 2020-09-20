#include "B_Air.h"
#include "../World/McWorld.h"
#include "Enums.h"


B_Air::B_Air()
{
	BlockID = 0;
	BlockName = TEXT("Air");
	BlockModelType = NONE;
}

UMaterialInstanceDynamic* B_Air::GetMaterial(UObject* UO)
{
	return nullptr;
}

TArray<uint8> B_Air::GetBinaryData()
{
	return {0, 0};
}

B_Block* B_Air::Clone()
{
	return this;          //BIG BIG BAD | NO GOOD
}
