


#include "ItemDrop.h"
#include "ProceduralMeshComponent/Public/ProceduralMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Item.h"
#include "Components/SphereComponent.h"

// Sets default values
AItemDrop::AItemDrop()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	InteractionZone = CreateDefaultSubobject<USphereComponent>(TEXT("ItemDrop Interaction Zone"));
	InteractionZone->SetSphereRadius(0);
	Mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Item Drop Mesh"));
	SM = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeFinder(TEXT("StaticMesh'/Game/Meshes/Misc/cube.cube'"));
	SM->SetStaticMesh(CubeFinder.Object);
	SetRootComponent(SM);
	Mesh->SetupAttachment(GetRootComponent());
	Mesh->AddLocalOffset({0, 0, 15});
	InteractionZone->SetCollisionProfileName(TEXT("NoCollision"));
	SM->SetCollisionProfileName(TEXT("NoCollision"));
	//SM->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	/*SM->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	SM->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	SM->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);*/

	//Mesh->SetWorldScale3D({.25, .25, .25});
	SM->SetWorldScale3D({.25, .25, .25});
	SM->SetVisibility(false);
	InteractionZone->SetupAttachment(SM);
}

void AItemDrop::ItemOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AItemDrop* Drop = Cast<AItemDrop>(OtherActor))
	{
		//if the 2 Items are stackable destroy the lower drop and add the that item count to the other drop
		if (Drop != this														//Avoid self-collision
			&& ItemStack.ItemS && ItemStack.Number &&							//make sure that this drop
			Drop->GetItemStack().ItemS && Drop->GetItemStack().Number			//and the other one are valid
			 &&
			ItemStack.ItemS->IsStackableWith(Drop->GetItemStack().ItemS))		//Is this Item stackable with the other
		{
			if (GetActorLocation().Z < Drop->GetActorLocation().Z)				//is this drop physically below the other one ? Yes then stack
			{
				Drop->IncreaseItemCount(ItemStack.Number);
				Drop->SetActorScale3D(Drop->GetActorScale3D() + ItemStack.Number / 128);
				Destroy();
			}
		}
	}
}

// Called when the game starts or when spawned
void AItemDrop::BeginPlay()
{
	Super::BeginPlay();
	
	Mesh->WakeAllRigidBodies();
}

void AItemDrop::EndPlay(EEndPlayReason::Type Reason)
{
}

// Called every frame
void AItemDrop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!InitComplete)
	{
		InteractionZone->SetSphereRadius(20 * 4);
		InteractionZone->OnComponentBeginOverlap.AddDynamic(this, &AItemDrop::ItemOverlap);
		InteractionZone->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECR_Overlap);
		SM->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
		SM->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
		SM->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		SM->SetEnableGravity(true);
		SM->SetSimulatePhysics(true);
		SM->WakeAllRigidBodies();
		InitComplete = true;
	}

	FRotator currRot = Mesh->GetComponentRotation();
	currRot.Pitch = 0;
	currRot.Roll = 0;
	currRot.Yaw += FMath::Fmod(45.f * DeltaTime, 360);
	Mesh->SetWorldRotation(currRot);
	Mesh->SetWorldRotation(currRot);
	Mesh->AddLocalOffset({0, 0, sinf(TimeElapsed)});

	TimeElapsed += DeltaTime; //TODO replace with timer
	if (TimeElapsed > 60 * 2)
		Destroy();
}

void AItemDrop::SetMesh(const TArray<FVector>& Verts, const TArray<int32>& Tris, const TArray<FVector2D>& UVs, const TArray<FVector>& Normals, UMaterialInstanceDynamic* Material)
{
	Mesh->CreateMeshSection(0, Verts, Tris, Normals, UVs, TArray<FColor>(), TArray<FProcMeshTangent>(), false);
	Mesh->SetMaterial(0, Material);
}

void AItemDrop::SetItemStack(FItemStack NewItemStack)
{
	ItemStack = NewItemStack;
}

void AItemDrop::UpdateItemCount(int32 NewCount)
{
	ItemStack.Number = NewCount;
	if (NewCount == 0)
		Destroy();
}

void AItemDrop::IncreaseItemCount(int32 AdditionalCount)
{
	ItemStack.Number += AdditionalCount;
}

FItemStack AItemDrop::GetItemStack()
{
	return ItemStack;
}
