#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlockSpaceChecker.generated.h"

UCLASS()
class MC_FAKE_API ABlockSpaceChecker : public AActor
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, 
		const FHitResult& SweepResult);

public:	
	ABlockSpaceChecker();

	bool bHitPlayer = false;

};
