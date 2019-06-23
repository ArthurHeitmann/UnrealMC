

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "ItemMeshComponent.generated.h"

/**
 * 
 */
UCLASS()
class MC_FAKE_API UItemMeshComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

private:
	static UStaticMesh* ItemMesh;
	static UMaterial* ItemMaterial;

public:
	UItemMeshComponent();
	void InitWithItem(class Item* Item);
};
