#include "DebugQuickPawn.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"

ADebugQuickPawn::ADebugQuickPawn()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(GetRootComponent());
}

void ADebugQuickPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADebugQuickPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADebugQuickPawn::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector() * Value);
}

void ADebugQuickPawn::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void ADebugQuickPawn::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void ADebugQuickPawn::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector() * Value);
}

void ADebugQuickPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("Move Forward"), this, &ADebugQuickPawn::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("Move Right"), this, &ADebugQuickPawn::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Look Up"), this, &ADebugQuickPawn::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Look Turn"), this, &ADebugQuickPawn::Turn);

}

