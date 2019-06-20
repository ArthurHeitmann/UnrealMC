


#include "ItemDrop.h"
#include "ProceduralMeshComponent/Public/ProceduralMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Item.h"

// Sets default values
AItemDrop::AItemDrop()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Item Drop Mesh"));
	SM = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeFinder(TEXT("StaticMesh'/Game/Meshes/Misc/cube.cube'"));
	SM->SetStaticMesh(CubeFinder.Object);
	SetRootComponent(SM);
	Mesh->SetupAttachment(GetRootComponent());
	Mesh->AddLocalOffset({0, 0, 15});
	SM->SetCollisionProfileName(TEXT(""));
	//SM->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	SM->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	SM->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	SM->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	SM->SetEnableGravity(true);
	SM->SetSimulatePhysics(true);
	//Mesh->SetWorldScale3D({.25, .25, .25});
	SM->SetWorldScale3D({.25, .25, .25});
	SM->SetVisibility(false);
}

// Called when the game starts or when spawned
void AItemDrop::BeginPlay()
{
	Super::BeginPlay();
	
	Mesh->WakeAllRigidBodies();
}

// Called every frame
void AItemDrop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FRotator currRot = Mesh->GetComponentRotation();
	currRot.Pitch = 0;
	currRot.Roll = 0;
	currRot.Yaw += FMath::Fmod(45.f * DeltaTime, 360);
	Mesh->SetWorldRotation(currRot);
	Mesh->SetWorldRotation(currRot);
	Mesh->AddLocalOffset({0, 0, sinf(TimeElapsed)});
	TimeElapsed += DeltaTime;
}

void AItemDrop::SetMesh(const TArray<FVector>& Verts, const TArray<int32>& Tris, const TArray<FVector2D>& UVs, const TArray<FVector>& Normals, UMaterialInstanceDynamic* Material)
{
	Mesh->CreateMeshSection(0, Verts, Tris, Normals, UVs, TArray<FColor>(), TArray<FProcMeshTangent>(), false);
	Mesh->SetMaterial(0, Material);
}

void AItemDrop::SetItem(Item* NewItem)
{
	ItemRef = NewItem;
}

