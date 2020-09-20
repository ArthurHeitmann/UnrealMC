#include "BlockBreaking.h"
#include "RuntimeMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "B_Block.h"
#include "RuntimeMeshProviderStatic.h"

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
	MeshProvider = NewObject<URuntimeMeshProviderStatic>(this, TEXT("Mesh Provider"));
	Mesh->Initialize(MeshProvider);
	CollisionMeshProvider = NewObject<URuntimeMeshProviderStatic>(this, TEXT("Collision Mesh Provider"));
	CollisionMesh->Initialize(CollisionMeshProvider);
}

void ABlockBreaking::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABlockBreaking::InitWithBlock(B_Block* NewBlock)
{
	BlockReference = NewBlock;
	if (MeshProvider->GetSectionIds(0).Num())
	{
		for (int32 section : MeshProvider->GetSectionIds(0))
			MeshProvider->RemoveSection(0, section);
	}
	
	TArray<FVector> Vertices(NewBlock->GetAllVertices(0.f, 0.f, 0.f));
	TArray<int32> Triangles(NewBlock->GetAllTrianglesFrom());
	TArray<FVector2D> UVs(NewBlock->GetAllUVs());
	TArray<FVector> Normals(NewBlock->GetAllNormals());

	MeshProvider->SetupMaterialSlot(0, FName(TEXT("")), NewBlock->GetMaterial(this));
	MeshProvider->CreateSectionFromComponents(0, 0, 0, Vertices, Triangles, Normals, UVs, TArray<FColor>(), TArray<FRuntimeMeshTangent>(), ERuntimeMeshUpdateFrequency::Infrequent, false);
	CollisionMeshProvider->CreateSectionFromComponents(0, 0, 0, Vertices, Triangles, Normals, UVs, TArray<FColor>(), TArray<FRuntimeMeshTangent>(), ERuntimeMeshUpdateFrequency::Infrequent, true);
	// Mesh->CreateMeshSection(0, Vertices, Triangles, Normals, UVs, TArray<FColor>(), TArray<FRuntimeMeshTangent>(), false);
	// Mesh->SetMaterial(0, NewBlock->GetMaterial(this));
	// CollisionMesh->CreateMeshSection(0, Vertices, Triangles, Normals, UVs, TArray<FColor>(), TArray<FRuntimeMeshTangent>(), true);		TODO remove
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

