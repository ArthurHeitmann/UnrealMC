

#pragma once

#include "CoreMinimal.h"
#include "ItemStack.h"
#include "RuntimeMeshComponent.h"
#include "ItemMeshComponent.generated.h"

/**
 * 
 */
UCLASS()
class MC_FAKE_API UItemMeshComponent : public USceneComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* ItemMesh;
	UPROPERTY(EditAnywhere)
	URuntimeMeshComponent* CustomMesh;
	UMaterialInstanceDynamic* ItemMaterial;
	UStaticMesh* TmpItemMesh;
	UMaterial* TmpItemMaterial;
	I_Item PreviousItem;
	bool bWasPreviousItemEmpty = true;
	FItemStack const ** ItemPointer;

	void OnItemChange();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunc) override;
	virtual void BeginPlay() override;
public:
	UItemMeshComponent();
	void SetItem(FItemStack const ** NewItemStackPointer);

	void SetItemMeshOffset(const FTransform & Offset);
	void SetCustomMeshOffset(const FTransform & Offset);
};