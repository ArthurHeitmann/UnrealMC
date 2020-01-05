#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DebugQuickPawn.generated.h"

UCLASS()
class MC_FAKE_API ADebugQuickPawn : public APawn
{
	GENERATED_BODY()

private:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void LookUp(float Value);
	void Turn(float Value);
	class UCameraComponent* Camera;
	UPROPERTY(EditAnywhere)
	USceneComponent* Root;

public:
	ADebugQuickPawn();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
