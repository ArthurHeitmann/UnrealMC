

#pragma once

#include "CoreMinimal.h"
#include "Components/MeshComponent.h"
#include "ItemStack.h"
#include "ItemMeshComponent.generated.h"

/**
 * 
 */
UCLASS()
class MC_FAKE_API UItemMeshComponent : public UMeshComponent
{
	GENERATED_BODY()

protected:
	UStaticMeshComponent* StaticMesh;
	UMaterial* ItemMaterial;
	Item* PreviousItem;
	FItemStack** ItemPointer;

	void OnItemChange();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunc) override;

public:
	UItemMeshComponent();
	void SetItem(FItemStack** NewItemStackPointer);
};
