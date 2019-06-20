#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemDrop.generated.h"

UCLASS()
class MC_FAKE_API AItemDrop : public AActor
{
	GENERATED_BODY()
	
private:
	USceneComponent* Root;
	float TimeElapsed;
	class Item* ItemRef;

protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* SM;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UProceduralMeshComponent* Mesh;

	AItemDrop();
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)
	void SetMesh(const TArray<FVector>& Verts, const TArray<int32>& Tris, const TArray<FVector2D>& UVs, const TArray<FVector>& Normals, UMaterialInstanceDynamic* Material);
	void SetItem(class Item* NewItem);
};
