#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemStack.h"
#include "ItemDrop.generated.h"

UCLASS()
class MC_FAKE_API AItemDrop : public AActor
{
	GENERATED_BODY()
	
protected:
	UPROPERTY()
	USceneComponent* Root;
	UPROPERTY(EditAnywhere)
	class USphereComponent* InteractionZone;
	float TimeElapsed;
	FItemStack ItemStack;
	bool InitComplete = false;

	//Item Pickup Anim
	UPROPERTY()
	class UTimelineComponent* TLComp;
	UPROPERTY()
	UCurveFloat* FloatCurve;
	UFUNCTION()
	void TimelineUpdate(float val);
	UFUNCTION()
	void TimelineEnd();
	FVector StartLoc;
	USceneComponent* TargetLoc;
	FVector TargetOffset;
	//end anim

	UFUNCTION()
	void ItemOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type Reason) override;
public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* SM;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class URuntimeMeshComponent* Mesh;

	AItemDrop();
	virtual void Tick(float DeltaTime) override;
	void SetMesh(const TArray<FVector>& Verts, const TArray<int32>& Tris, const TArray<FVector2D>& UVs, const TArray<FVector>& Normals, UMaterialInstanceDynamic* Material);
	void SetItemStack(FItemStack NewItemStack);
	void UpdateItemCount(int32 NewCount, USceneComponent* TargetLocation = nullptr, FVector Offset = FVector::ZeroVector);
	void IncreaseItemCount(int32 AdditionalCount);
	FItemStack GetItemStack();
};
