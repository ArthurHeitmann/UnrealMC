#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "../../Items/ItemStack.h"
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
	void InitSelectedItemMesh(USceneComponent* AttachTo);
	void InitSelectedItemMesh(USceneComponent* AttachTo, const FTransform & Offset1, const FTransform & Offset2);
	void InitItemSlots(int32 Num);
	void InitUI(class AGameModeBase* Gamemode);

	void SelectItemSlot(int32 Num);
	void SelectNextItem();
	void SelectPreviousItem();

	void SetSelectedItemPointer(FItemStack const ** NewISPointer);
	void SetItemWheelVisibility(bool State);

	UPROPERTY(EditAnywhere)
	class UItemSlots* Slot_BasicInventory;
protected:
	UPROPERTY(EditAnywhere)
	class UUI_QuickAccessSlots* QuickAccessSlots;
	UPROPERTY(EditAnywhere)
	class UBoxComponent* ItemPickupBox;
	UPROPERTY(EditAnywhere)
	class UItemMeshComponent* SelectedItemMesh;
	FItemStack const ** SelectedItemRef;

	virtual void BeginPlay() override;

	UFUNCTION()
		void ItemPickBoxTrigger(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
