

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
	class UItemSlots* Slot_QuickAcces;


	UItemSystemComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FItemStack AddItemStackToInventory(FItemStack Items);
	void InitQuickAccessSlots(int32 Num);

protected:
	UPROPERTY(EditAnywhere)
		class UBoxComponent* ItemPickupBox;

	virtual void BeginPlay() override;

	UFUNCTION()
		void ItemPickBoxTrigger(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


};
