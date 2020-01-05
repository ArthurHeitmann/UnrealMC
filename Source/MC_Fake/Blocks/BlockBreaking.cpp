#include "BlockBreaking.h"
#include "RuntimeMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "B_Block.h"

ABlockBreaking::ABlockBreaking()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<URuntimeMeshComponent>(TEXT("Breaking Block Mesh"));
	CollisionMesh = CreateDefaultSubobject<URuntimeMeshComponent>(TEXT("Breaking Block Collision Mesh"));
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	Mesh->SetupAttachment(GetRootComponent());
	CollisionMesh->SetupAttachment(GetRootComponent());
	CollisionMesh->SetVisibility(false);
}

void ABlockBreaking::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABlockBreaking::InitWithBlock(B_Block* NewBlock)
{
	BlockReference = NewBlock;
	if (Mesh->GetNumSections())
		Mesh->ClearAllMeshSections();
	
	TArray<FVector> Vertices(NewBlock->GetAllVertices(0.f, 0.f, 0.f));
	TArray<int32> Triangles(NewBlock->GetAllTrianglesFrom());
	TArray<FVector2D> UVs(NewBlock->GetAllUVs());
	TArray<FVector> Normals(NewBlock->GetAllNormals());

	Mesh->CreateMeshSection(0, Vertices, Triangles, Normals, UVs, TArray<FColor>(), TArray<FRuntimeMeshTangent>(), false);
	Mesh->SetMaterial(0, NewBlock->GetMaterial(this));
	CollisionMesh->CreateMeshSection(0, Vertices, Triangles, Normals, UVs, TArray<FColor>(), TArray<FRuntimeMeshTangent>(), true);
}

void ABlockBreaking::SetTimePassed(float Seconds)
{
	float percentage = fmax(0.f, fmin(1.f, Seconds / BlockReference->GetBreakTime()));
	Mesh->SetWorldScale3D(FVector(1, 1, 1) * (1.f - percentage * 0.05));
}

B_Block* ABlockBreaking::GetBlock()
{
	return BlockReference;
}

