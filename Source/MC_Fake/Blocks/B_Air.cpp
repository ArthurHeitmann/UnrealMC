// Fill out your copyright notice in the Description page of Project Settings.


#include "B_Air.h"
#include "../World/McWorld.h"
#include "Enums.h"

B_Air* B_Air::AirRef;

B_Air::B_Air()
{
	BlockID = 0;
	BlockName = TEXT("Air");
	BlockModelType = NONE;
	BlockEnum = BAir;
}

UMaterialInstanceDynamic* B_Air::GetMaterial(UObject* UO)
{
	return nullptr;
}

TArray<uint8> B_Air::GetBinaryData()
{
	return {0, 0};
}
