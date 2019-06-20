// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../World/McWorld.h"
#include "Enums.h"

class MC_FAKE_API Block
{

	struct ItemDrops { float Chance; class Item* Item; };

protected:
	static UMaterial* BlockMaterial;
	uint16 BlockID;
	uint8 SubID;
	FName BlockName;
	TEnumAsByte<EBlockModelType> BlockModelType;
	//FName TextureName;
	TEnumAsByte<EAllBlocks> BlockEnum;
	class UTexture* Texture;
	float BreakTime;
	TArray<ItemDrops> PossibleItemDrops;

	void DropItem(class UWorld* World, FVector Location, class Item* DropItem);

public:
	Block();
	virtual ~Block();

	static Block* GetBlockFromBytes(uint8* Bytes);

	virtual bool IsSideOptimizable(TEnumAsByte<EFaceDirection> Direction);
	virtual bool IsBlockOpaque();
	virtual bool UsesCustomModel();
	virtual bool HasConstantMaterial();	//Actually use this method
	virtual TEnumAsByte<EBlockModelType> GetBlockModelType();
	virtual TEnumAsByte<EAllBlocks> GetBlockEnum();
	virtual class UMaterialInstanceDynamic* GetMaterial(UObject* UObj);
	//virtual class UTexture* GetTexture();
	float GetBreakTime();
	Block GetSelf();
	virtual TArray<uint8> GetBinaryData();

	virtual void OnPlace(class UWorld* World, FVector Location);
	virtual void OnLeftclick(class UWorld* World, FVector Location);
	virtual void OnBeginBreak(class UWorld* World, FVector Location);
	virtual void Updatebreak(float Millis);
	virtual void OnBreak(class UWorld* World, FVector Location);

	virtual TArray<FVector> GetTopVertecies(float x, float y, float z);
	virtual TArray<FVector> GetRightVertecies(float x, float y, float z);
	virtual TArray<FVector> GetBottomVertecies(float x, float y, float z);
	virtual TArray<FVector> GetLeftVertecies(float x, float y, float z);
	virtual TArray<FVector> GetFrontVertecies(float x, float y, float z);
	virtual TArray<FVector> GetBackVertecies(float x, float y, float z);

	virtual TArray<int32> GetTopTrianglesFrom(int32 Start);
	virtual TArray<int32> GetRightTrianglesFrom(int32 Start);
	virtual TArray<int32> GetBottomTrianglesFrom(int32 Start);
	virtual TArray<int32> GetLeftTrianglesFrom(int32 Start);
	virtual TArray<int32> GetFrontTrianglesFrom(int32 Start);
	virtual TArray<int32> GetBackTrianglesFrom(int32 Start);

	virtual TArray<FVector2D> GetTopUVs();
	virtual TArray<FVector2D> GetRightUVs();
	virtual TArray<FVector2D> GetBottomUVs();
	virtual TArray<FVector2D> GetLeftUVs();
	virtual TArray<FVector2D> GetFrontUVs();
	virtual TArray<FVector2D> GetBackUVs();

	virtual TArray<FVector> GetTopNormals();
	virtual TArray<FVector> GetRightNormals();
	virtual TArray<FVector> GetBottomNormals();
	virtual TArray<FVector> GetLeftNormals();
	virtual TArray<FVector> GetFrontNormals();
	virtual TArray<FVector> GetBackNormals();

	virtual TArray<FVector> GetAllVertecies(float x, float y, float z);
	virtual TArray<FVector2D> GetAllUVs();
	virtual TArray<FVector> GetAllNormals();
	virtual TArray<int32> GetAllTrainglesFrom(int32 Start = 0);
};