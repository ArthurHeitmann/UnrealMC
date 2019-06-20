// Fill out your copyright notice in the Description page of Project Settings.


#include "DebugQuickPawn.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
ADebugQuickPawn::ADebugQuickPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void ADebugQuickPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADebugQuickPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADebugQuickPawn::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector() * Value);
}

void ADebugQuickPawn::LookUp(float value)
{
	AddControllerPitchInput(value);
}

void ADebugQuickPawn::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void ADebugQuickPawn::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector() * Value);
}

// Called to bind functionality to input
void ADebugQuickPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("Move Forward"), this, &ADebugQuickPawn::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("Move Right"), this, &ADebugQuickPawn::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Look Up"), this, &ADebugQuickPawn::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Look Turn"), this, &ADebugQuickPawn::Turn);

}

