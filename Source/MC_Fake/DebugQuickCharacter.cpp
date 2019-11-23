#include "DebugQuickCharacter.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "World/WorldLoadingComponent.h"
#include "Engine/World.h"
#include "Blocks/BlockBreaking.h"
#include "LineTraceingInteractions.h"
#include "World/McWorld.h"
#include "Kismet/GameplayStatics.h"
#include "Items/I_Pickaxe_Stone.h"
#include "Items/I_BlockItem.h"
#include "Blocks/B_Stone.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Player/Inventory/ItemSystemComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ADebugQuickCharacter::ADebugQuickCharacter()
{
	I_Pickaxe_Stone();

	PrimaryActorTick.bCanEverTick = true;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(GetRootComponent());
	Camera->AddLocalOffset({-25, 0, 55});
	Camera->bUsePawnControlRotation = true;
	WorldLoader = CreateDefaultSubobject<UWorldLoadingComponent>(TEXT("World Loader"));
	LineTracer = CreateDefaultSubobject<ULineTraceingInteractions>(TEXT("Line Tracing Component"));
	LineTracer->SetCamera(Camera);

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

void ADebugQuickCharacter::ToggleItemWheel()
{
	if (UGameplayStatics::IsGamePaused(GetWorld()) || !ItemSystem)
		return;

	ItemSystem->ToggleItemWheel();
}

void ADebugQuickCharacter::ScrollUp()
{
	if (ItemSystem)
		ItemSystem->SelectNextItem();
}

void ADebugQuickCharacter::ScrollDown()
{
	if (ItemSystem)
		ItemSystem->SelectPreviousItem();
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

void ADebugQuickCharacter::SelectItem(int32 numb)
{
	ItemSystem->SelectItemSlot(numb);
}

void ADebugQuickCharacter::StartCrouch()
{
	Crouch();
}

void ADebugQuickCharacter::EndCrouch()
{
	UnCrouch();
}

void ADebugQuickCharacter::ToggleChunkBoarders()
{
	UE_LOG(LogTemp, Error, TEXT("Feature currently unsupported"));
	/*UWorld* W = GetWorld();
	if (!W)
		return;
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(W, Chunk::StaticClass(), FoundActors);
	for (AActor* Chunk : FoundActors)
	{
		Cast<Chunk>(Chunk)->ToggleChunkBorders();
	}*/
}

void ADebugQuickCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	FItemStack ** SelectedItemPointer = new FItemStack*;
	ItemSystem->SetSelectedItemPointer(const_cast<FItemStack const **>(SelectedItemPointer));
	ItemSystem->InitItemSlots(10);
	ItemSystem->InitUI(GetWorld()->GetAuthGameMode());
	ItemSystem->InitPickUpBox({ 125, 125, 125 });

	FVector TransOffset1(38, 45, -45);
	FRotator RotOffset1(0, 90, 0);
	FVector ScaleTr1(0.9, 0.9, 0.9);

	FVector TransOffset2(27, 28, -34);
	FRotator RotOffset2(0, 315, 0);
	FVector ScaleTr2(0.25, 0.25, 0.25);
	//ItemSystem->InitSelectedItemMesh(Camera, FTransform(RotOffset1, TransOffset1, ScaleTr1), FTransform(RotOffset2, TransOffset2, ScaleTr2));
	ItemSystem->SetSelectedItemPointer(const_cast<const FItemStack * *>(SelectedItemPointer));
	LineTracer->SetSelectedItemPointer(SelectedItemPointer);
}

// Called every frame
void ADebugQuickCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	PlayerInputComponent->BindAction<FSelectItemDelegate>(TEXT("Item1"), IE_Pressed, this, &ADebugQuickCharacter::SelectItem, 0);
	PlayerInputComponent->BindAction<FSelectItemDelegate>(TEXT("Item2"), IE_Pressed, this, &ADebugQuickCharacter::SelectItem, 1);
	PlayerInputComponent->BindAction<FSelectItemDelegate>(TEXT("Item3"), IE_Pressed, this, &ADebugQuickCharacter::SelectItem, 2);
	PlayerInputComponent->BindAction<FSelectItemDelegate>(TEXT("Item4"), IE_Pressed, this, &ADebugQuickCharacter::SelectItem, 3);
	PlayerInputComponent->BindAction<FSelectItemDelegate>(TEXT("Item5"), IE_Pressed, this, &ADebugQuickCharacter::SelectItem, 4);
	PlayerInputComponent->BindAction<FSelectItemDelegate>(TEXT("Item6"), IE_Pressed, this, &ADebugQuickCharacter::SelectItem, 5);
	PlayerInputComponent->BindAction<FSelectItemDelegate>(TEXT("Item7"), IE_Pressed, this, &ADebugQuickCharacter::SelectItem, 6);
	PlayerInputComponent->BindAction<FSelectItemDelegate>(TEXT("Item8"), IE_Pressed, this, &ADebugQuickCharacter::SelectItem, 7);
	PlayerInputComponent->BindAction<FSelectItemDelegate>(TEXT("Item9"), IE_Pressed, this, &ADebugQuickCharacter::SelectItem, 8);
	PlayerInputComponent->BindAction<FSelectItemDelegate>(TEXT("Item10"),IE_Pressed, this, &ADebugQuickCharacter::SelectItem, 9);
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &ADebugQuickCharacter::StartCrouch);
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Released, this, &ADebugQuickCharacter::EndCrouch);
	PlayerInputComponent->BindAction(TEXT("ShowChunkBoarders"), IE_Pressed, this, &ADebugQuickCharacter::ToggleChunkBoarders);
	PlayerInputComponent->BindAction(TEXT("ScrollUp"), IE_Pressed, this, &ADebugQuickCharacter::ScrollUp);
	PlayerInputComponent->BindAction(TEXT("ScrollDown"), IE_Pressed, this, &ADebugQuickCharacter::ScrollDown);
	PlayerInputComponent->BindAction(TEXT("ToggleItemWheel"), IE_Pressed, this, &ADebugQuickCharacter::ToggleItemWheel);
	PlayerInputComponent->BindAction(TEXT("ToggleItemWheel"), IE_Released, this, &ADebugQuickCharacter::ToggleItemWheel);

	PlayerInputComponent->BindAxis(TEXT("Move Forward"), this, &ADebugQuickCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("Move Right"), this, &ADebugQuickCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ADebugQuickCharacter::Turn);
	PlayerInputComponent->BindAxis(TEXT("Look Up"), this, &ADebugQuickCharacter::LookUp);
}

