

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "ItemStack.h"
#include "ItemMeshComponent.generated.h"

/**
 * 
 */
UCLASS()
class MC_FAKE_API UItemMeshComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	//UStaticMeshComponent* StaticMesh;
	UMaterialInstanceDynamic* ItemMaterial;
	Item PreviousItem;
	bool bWasPreviouseItemEmpty = true;
	FItemStack** ItemPointer;

	void OnItemChange();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunc) override;

public:
	UItemMeshComponent();
	void SetItem(FItemStack** NewItemStackPointer);
};
