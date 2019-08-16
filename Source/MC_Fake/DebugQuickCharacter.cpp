#include "DebugQuickCharacter.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "World/WorldLoadingComponent.h"
#include "Engine/World.h"
#include "Blocks/BlockBreaking.h"
#include "LineTraceingInteractions.h"
#include "World/McWorld.h"
#include "Kismet/GameplayStatics.h"
#include "Items/ItemMeshComponent.h"
#include "Items/I_Pickaxe_Stone.h"
#include "Items/I_BlockItem.h"
#include "Blocks/B_Stone.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Player/ItemSystemComponent.h"

// Sets default values
ADebugQuickCharacter::ADebugQuickCharacter()
{
	I_Pickaxe_Stone();

	PrimaryActorTick.bCanEverTick = true;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	CurrentItemMesh = CreateDefaultSubobject<UItemMeshComponent>(TEXT("Mesh of current Item"));
	CurrentItemMesh->SetWorldScale3D({.9, .9, .9});
	CurrentItemMesh->AddRelativeRotation({ 0, 30, -50 });
	CurrentItemMesh->AddRelativeRotation(FRotator{0, 90, 0});
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(GetRootComponent());
	Camera->AddLocalOffset({-25, 0, 55});
	Camera->bUsePawnControlRotation = true;
	WorldLoader = CreateDefaultSubobject<UWorldLoadingComponent>(TEXT("World Loader"));
	LineTracer = CreateDefaultSubobject<ULineTraceingInteractions>(TEXT("Line Tracing Component"));
	LineTracer->SetCamera(Camera);
	CurrentItemMesh->SetupAttachment(Camera);

	ItemSystem = CreateDefaultSubobject<UItemSystemComponent>(TEXT("Item System"));
	ItemSystem->SetupAttachment(GetRootComponent());
}

void ADebugQuickCharacter::MoveForward(float v)
{
	AddMovementInput(GetActorForwardVector() * v);
}

void ADebugQuickCharacter::MoveRight(float v)
{
	AddMovementInput(GetActorRightVector() * v);
}

void ADebugQuickCharacter::LookUp(float v)
{
	AddControllerPitchInput(v);
}

void ADebugQuickCharacter::Turn(float v)
{
	AddControllerYawInput(v);
}

void ADebugQuickCharacter::LeftClick()
{
	LineTracer->LeftClickStart();
}

void ADebugQuickCharacter::LeftClickEnd()
{
	LineTracer->LeftClickEnd();
}

void ADebugQuickCharacter::RightClick()
{
	LineTracer->RightClickStart();
}

void ADebugQuickCharacter::Save()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), TSubclassOf<AMcWorld>(AMcWorld::StaticClass()), FoundActors);
	Cast<AMcWorld>(FoundActors[0])->QuickSave();
}

void ADebugQuickCharacter::Load()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), TSubclassOf<AMcWorld>(AMcWorld::StaticClass()), FoundActors);
	Cast<AMcWorld>(FoundActors[0])->QuickLoad();
}

void ADebugQuickCharacter::SelectItem1()
{
	*CurrentItem = new I_Pickaxe_Stone();
}

void ADebugQuickCharacter::SelectItem2()
{
	*CurrentItem = new I_BlockItem(new B_Stone());
}

void ADebugQuickCharacter::StartCrouch()
{
	Crouch();
}

void ADebugQuickCharacter::EndCrouch()
{
	UnCrouch();
}

// Called when the game starts or when spawned
void ADebugQuickCharacter::BeginPlay()
{
	Super::BeginPlay();
	CurrentItem = new Item*;
	*CurrentItem = new I_Pickaxe_Stone();
	CurrentItemMesh->InitWithItem(*CurrentItem);
	LineTracer->SetCurrentItem(CurrentItem);
	PreviousItem = *CurrentItem;

	ItemSystem->InitQuickAccessSlots(10);
	ItemSystem->InitPickUpBox( { 125, 125, 125 } );
}

// Called every frame
void ADebugQuickCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (*CurrentItem != PreviousItem)
	{
		CurrentItemMesh->InitWithItem(*CurrentItem);
		PreviousItem = *CurrentItem;
	}
}

// Called to bind functionality to input
void ADebugQuickCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("Left Click"), IE_Pressed, this, &ADebugQuickCharacter::LeftClick);
	PlayerInputComponent->BindAction(TEXT("Left Click"),  IE_Released, this, &ADebugQuickCharacter::LeftClickEnd);
	PlayerInputComponent->BindAction(TEXT("Right Click"), IE_Pressed, this, &ADebugQuickCharacter::RightClick);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ADebugQuickCharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &ADebugQuickCharacter::StopJumping);
	PlayerInputComponent->BindAction(TEXT("Save"), IE_Pressed, this, &ADebugQuickCharacter::Save);
	PlayerInputComponent->BindAction(TEXT("Load"), IE_Pressed, this, &ADebugQuickCharacter::Load);
	PlayerInputComponent->BindAction(TEXT("Item1"), IE_Pressed, this, &ADebugQuickCharacter::SelectItem1);
	PlayerInputComponent->BindAction(TEXT("Item2"), IE_Pressed, this, &ADebugQuickCharacter::SelectItem2);
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &ADebugQuickCharacter::StartCrouch);
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Released, this, &ADebugQuickCharacter::EndCrouch);

	PlayerInputComponent->BindAxis(TEXT("Move Forward"), this, &ADebugQuickCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("Move Right"), this, &ADebugQuickCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ADebugQuickCharacter::Turn);
	PlayerInputComponent->BindAxis(TEXT("Look Up"), this, &ADebugQuickCharacter::LookUp);
}

