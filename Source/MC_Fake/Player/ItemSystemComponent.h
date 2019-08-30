

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "../Items/ItemStack.h"
#include "ItemSystemComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MC_FAKE_API UItemSystemComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	UItemSystemComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FItemStack AddItemStackToInventory(FItemStack Items);
	void InitPickUpBox(const FVector& BoxExtent);
	void InitSelectedItemMesh(USceneComponent* AttachTo, const FTransform & Offset1 = FTransform(), const FTransform & Offset2 = FTransform());
	void InitQuickAccessSlots(int32 Num);

	void SelectItemSlot(int32 num);
	void SelectNextItem();
	void SelectPreviousItem();

	void SetSelectedItemPointer(FItemStack** NewISPointer);

protected:
	UPROPERTY()
	class UQuickAccesSlots* Slot_QuickAcces;
	UPROPERTY(EditAnywhere)
		class UBoxComponent* ItemPickupBox;
	UPROPERTY(EditAnywhere)
		class UItemMeshComponent* SelectedItemMesh;
	FItemStack** SelectedItemRef;

	virtual void BeginPlay() override;

	UFUNCTION()
		void ItemPickBoxTrigger(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
