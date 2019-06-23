#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemDrop.generated.h"

UCLASS()
class MC_FAKE_API AItemDrop : public AActor
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	USceneComponent* Root;
	UPROPERTY(EditAnywhere)
	class USphereComponent* InteractionZone;
	float TimeElapsed;
	class Item* ItemRef;
	int32 ItemCount;
	UFUNCTION()
	void ItemOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	bool InitComplete = false;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type Reason) override;
public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* SM;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UProceduralMeshComponent* Mesh;

	AItemDrop();
	virtual void Tick(float DeltaTime) override;
	void SetMesh(const TArray<FVector>& Verts, const TArray<int32>& Tris, const TArray<FVector2D>& UVs, const TArray<FVector>& Normals, UMaterialInstanceDynamic* Material);
	void SetItem(class Item* NewItem);
	Item* GetItem();
};
