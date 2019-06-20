// Fill out your copyright notice in the Description page of Project Settings.

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
	void LookUp(float value);
	void Turn(float Value);
	class UCameraComponent* Camera;
	UPROPERTY(EditAnywhere)
	USceneComponent* Root;

public:
	// Sets default values for this pawn's properties
	ADebugQuickPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
