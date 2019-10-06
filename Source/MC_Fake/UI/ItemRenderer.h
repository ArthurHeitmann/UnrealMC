

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemRenderer.generated.h"

UCLASS()
class MC_FAKE_API AItemRenderer : public AActor
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere)
	class USceneCaptureComponent2D* SceneCapture;
	UPROPERTY(EditAnywhere)
	class UItemMeshComponent* ItemMesh;
	UPROPERTY(EditAnywhere)
	class UMaterialInstanceDynamic* UIMaterial;
	UPROPERTY(EditAnywhere)
	class UTextureRenderTarget2D* RenderTarget;
	UPROPERTY(EditAnywhere)
	class UMaterial* UIMaterialLoaded;

	struct FItemStack const ** PPIS;


	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type Reason) override;

public:	
	virtual void Tick(float DeltaTime) override;
	AItemRenderer();

	void InitWithItemStack(const FItemStack& ItemStack);
	class UMaterialInstanceDynamic* GetUIMaterial(UObject* MatOwner);

};
