#pragma once

#include "CoreMinimal.h"

#include "GameFrameWork/Character.h"
#include "Character_Boxed.generated.h"


class UBoxComponent;


UCLASS(config=Game, BlueprintType, meta=(ShortTooltip="A character is a type of Pawn that includes the ability to walk around."))
class MC_FAKE_API ACharacter_Boxed : public ACharacter
{
	GENERATED_BODY()
protected:
	UPROPERTY(Category=Character, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	UBoxComponent* BoxComponent;
	static FName BoxComponentName;

	virtual float GetScaledBoxHalfHeight(UBoxComponent* Box) const;
	virtual float GetScaledBoxAverageRadius(UBoxComponent* Box) const;
	
public:
	/** Default UObject constructor. */
	ACharacter_Boxed(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void PostInitializeComponents() override;

	virtual void GetSimpleCollisionCylinder(float& CollisionRadius, float& CollisionHalfHeight) const override;

	virtual void UpdateNavigationRelevance() override;

	virtual float GetDefaultHalfHeight() const override;

	virtual FVector GetNavAgentLocation() const override;
};
