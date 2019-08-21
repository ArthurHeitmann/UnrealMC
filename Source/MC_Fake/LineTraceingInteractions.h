

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/ItemStack.h"
#include "LineTraceingInteractions.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MC_FAKE_API ULineTraceingInteractions : public UActorComponent
{
	GENERATED_BODY()

private:	
	UPROPERTY(EditAnywhere)
	float HandReachDistance;
	FHitResult LeftClickResult;
	FHitResult RightClickResult;
	class Block* HittingBlock;
	class AChunk* HittingChunk;
	class UCameraComponent* RayShootingCamera;
	bool bIsHitting;
	FItemStack** CurrentItem;
	class AMcWorld* World;

protected:
	virtual void BeginPlay() override;

public:	
	ULineTraceingInteractions();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void LeftClickStart();
	void LeftClickEnd();
	void RightClickStart();
	void RightClickEnd();
	void SetCamera(class UCameraComponent* Cam);
	void SetSelectedItemPointer(FItemStack** NewPointer);
};
