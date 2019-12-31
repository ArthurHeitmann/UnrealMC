

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/ItemStack.h"
#include "LineTracingInteractions.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MC_FAKE_API ULineTracingInteractions : public UActorComponent
{
	GENERATED_BODY()

private:	
	UPROPERTY(EditAnywhere)
	float HandReachDistance;
	//FHitResult LeftClickResult;
	//FHitResult RightClickResult;
	class B_Block* HittingBlock;
	class B_Block** InitialBlock;
	class ABlockBreaking* BreakingIndicator;
	class ChunkCube* HittingCCube;
	FVector HittingBlockCoordinates;
	FIntVector HittingCoordinates;
	class Item* LastUsedItem;
	float TimeSpentHitting;

	class UCameraComponent* RayShootingCamera;
	bool bIsHitting;
	FItemStack** CurrentItem;
	class AMcWorld* McWorld;

	void RegisterNewHitAt(const FHitResult& hit);
	void ContinueBreaking();
	void CancelBreaking();
	FORCEINLINE float HitHelper(float NormalAxisVal, float Val);

protected:
	virtual void BeginPlay() override;

public:	
	ULineTracingInteractions();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void LeftClickStart();
	void LeftClickEnd();
	void RightClickStart();
	void RightClickEnd();
	void SetCamera(class UCameraComponent* Cam);
	void SetSelectedItemPointer(FItemStack ** NewPointer);
};