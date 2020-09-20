#include "ChunkCube.h"
#include "../Blocks/B_Block.h"
#include "RuntimeMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "McWorld.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "ChunkMeshGenerator.h"
#include "RuntimeMeshProviderStatic.h"

UChunkCube::UChunkCube()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UChunkCube::Init(FChunkFormCoords3D pPos, AMcWorld* pMcWorld, UChunk* pParentChunk)
{
	Pos = pPos;
	McWorld = pMcWorld;
	ParentChunk = pParentChunk;
	
	AddLocalOffset({ Pos.X * 1600.f, Pos.Y * 1600.f, 0 });
	
	ChunkMesh = NewObject<URuntimeMeshComponent>(this);
	ChunkMesh->SetupAttachment(this);
	ChunkMesh->RegisterComponent();
	CustomCollisionMesh = NewObject<URuntimeMeshComponent>(this);
	CustomCollisionMesh->SetupAttachment(this);
	CustomCollisionMesh->RegisterComponent();
	MeshProvider = NewObject<URuntimeMeshProviderStatic>(this);
	ChunkMesh->Initialize(MeshProvider);
	BoundingBox = NewObject<UBoxComponent>(this);
	BoundingBox->SetupAttachment(this);
	BoundingBox->RegisterComponent();
	BoundingBox->SetBoxExtent({ 800.f, 800.f, 800.f });
	BoundingBox->AddRelativeLocation({ 800.f, 800.f, 800.f });
	BoundingBox->bHiddenInGame = true;
}

void UChunkCube::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	bHasBeenDestroyed = true;
	Super::EndPlay(EndPlayReason);
	
	if (EndPlayReason != EEndPlayReason::Destroyed)
		return;
	
	//wait for block data to finish generating
	while (bIsGenerating)
		FPlatformProcess::Sleep(0.01);

	TArray<USceneComponent*> Comps;
	GetChildrenComponents(true, Comps);
	for (auto& Comp : Comps) {
		Comp->DestroyComponent();
	}
	
	//Remove all associations with this chunks in the world and other chunks 
	McWorld->RemoveLoadedChunkCube(Pos);

	for (int x = 0; x < BlockData.Num(); ++x)
	{
		for (int y = 0; y < BlockData[x].Num(); ++y)
		{
			for (int z = 0; z < BlockData[x][y].Num(); ++z)
			{
				if (BlockData[x][y][z] && BlockData[x][y][z]->GetName() != "Air")
					delete BlockData[x][y][z];
			}
		}
	}

	if (CubeNeighbors.North)
		CubeNeighbors.North->UpdateCubeNeighbor(SOUTH, nullptr, false);
	if (CubeNeighbors.East)
		CubeNeighbors.East->UpdateCubeNeighbor(WEST, nullptr, false);
	if (CubeNeighbors.South)
		CubeNeighbors.South->UpdateCubeNeighbor(NORTH, nullptr, false);
	if (CubeNeighbors.West)
		CubeNeighbors.West->UpdateCubeNeighbor(EAST, nullptr, false);
	if (CubeNeighbors.Top)
		CubeNeighbors.Top->UpdateCubeNeighbor(BOTTOM, nullptr, false);
	if (CubeNeighbors.Bottom)
		CubeNeighbors.Bottom->UpdateCubeNeighbor(TOP, nullptr, false);
}

void UChunkCube::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	int32 t21 = FDateTime::Now().GetMillisecond();
	if (bHasMeshDataChanged && bHasFinishedGenerating)
		UpdateMesh();
	int32 t22 = FDateTime::Now().GetMillisecond();
}

void UChunkCube::UpdateMesh()
{
	if (!bHasFinishedGenerating || !BlockData.Num())
		return;

	MeshLock.Lock();

	
	int numFace = 0;
	for (auto i = Vertices.CreateConstIterator(); i; ++i)
	{
		uint16 key = i.Key();
		/*UE_LOG(LogTemp, Warning, TEXT("Z: %d cbe: %d Verts: %d Tris: %d Norms: %d UVs: %d"), 
		Pos.Z,
		key, 
		Vertices[key].Num(),
		Triangles[key].Num(),
		Normals[key].Num(),
		UVs[key].Num());*/
		int32 t41 = FDateTime::Now().GetMillisecond();
		if (i.Value().Num())
		{
			MeshProvider->SetupMaterialSlot(
             key, 
             FName(),
             Blocks[key]->GetMaterial(ChunkMesh)
         );
			MeshProvider->CreateSectionFromComponents(
                0,
                key,
                key,
                i.Value(),
                Triangles[key],
                Normals[key],
                UVs[key],
                TArray<FColor>(),
                TArray<FRuntimeMeshTangent>(),
                ERuntimeMeshUpdateFrequency::Average,
                Blocks[key]->HasCollision()
                );
		}
		else if (MeshProvider->GetSectionIds(0).Contains(key))
		{
			MeshProvider->RemoveSection(0, key);
		}
		// ChunkMesh->CreateMeshSection(
		//           key,
		//           i.Value(),
		//           Triangles[key],
		//           Normals[key],
		//           UVs[key],
		//           TArray<FColor>(),
		//           TArray<FRuntimeMeshTangent>(),
		//           Blocks[key]->HasCollision());
		int32 t42 = FDateTime::Now().GetMillisecond();
		ProcMeshGenTime += t42 - t41;
		// ChunkMesh->SetMaterial(key, Blocks[key]->GetMaterial(ChunkMesh));
		numFace += Vertices[key].Num() / 4;
	}
	//UE_LOG(LogTemp, Warning, TEXT("ms/face: %f\n"), ((float) t12 - t11) / numFace);
	// UE_LOG(LogTemp, Warning, TEXT("Create %d faces"), numFace);


	bHasMeshDataChanged = false;
	MeshLock.Unlock();
}

bool UChunkCube::HasBeenDestroyed()
{
	return bHasBeenDestroyed;
}

FChunkFormCoords3D UChunkCube::GetPos()
{
	return Pos;
}

TArray<TArray<TArray<class B_Block*>>>& UChunkCube::GetBlockData()
{
	return BlockData;
}

void UChunkCube::SetNextGenerationStage(int NewStage)
{
	NextGenerationStage = NewStage;
}

int UChunkCube::GetNextGenerationStage()
{
	return NextGenerationStage;
}

UChunk* UChunkCube::GetParentChunk()
{
	return ParentChunk;
}

void UChunkCube::SetParentChunk(UChunk* PChunk)
{
	ParentChunk = PChunk;
}

void UChunkCube::SetHasMeshDataChanged(bool val)
{
	bHasMeshDataChanged = val;
}

void UChunkCube::SetHasBlockDataChanged(bool val)
{
	//MeshLock.Lock();
	//bHasBlockDataChanged = val;
	//MeshLock.Unlock();

	if (val && !bIsMeshGenPending)
	{
		McWorld->AddMeshGenTask(this);
		bIsMeshGenPending = true;
	}
}

bool UChunkCube::GetHasFinishedGenerating()
{
	return bHasFinishedGenerating;
}

void UChunkCube::SetHasFinishedGenerating(bool val)
{
	bHasFinishedGenerating = val;
}

void UChunkCube::SetIsMeshGenPending(bool val)
{
	bIsMeshGenPending = val;
}

bool UChunkCube::GetIsGenerating()
{
	return bIsGenerating;
}

void UChunkCube::SetIsGenerating(bool val)
{
	bIsGenerating = val;
}

B_Block*& UChunkCube::GetBlockAt(int x, int y, int z)
{
	return BlockData[x][y][z];
}

ChunkCubeGenData& UChunkCube::GetChunkCubeGenData()
{
	return CubeData;
}

ChunkCubeNeighbors& UChunkCube::GetChunkCubeNeighbors()
{
	return CubeNeighbors;
}

void UChunkCube::UpdateCubeNeighbor(EDirection NeighborSide, UChunkCube* NewNeighbor, bool bUpdateMesh)
{
	switch (NeighborSide)
	{
	case NORTH:
		CubeNeighbors.North = NewNeighbor;
		break;
	case EAST:
		CubeNeighbors.East = NewNeighbor;
		break;
	case SOUTH:
		CubeNeighbors.South = NewNeighbor;
		break;
	case WEST:
		CubeNeighbors.West = NewNeighbor;
		break;
	case TOP:
		CubeNeighbors.Top = NewNeighbor;
		break;
	case BOTTOM:
		CubeNeighbors.Bottom = NewNeighbor;
		break;
	}
	
	if (bUpdateMesh && bHasFinishedGenerating)
		SetHasBlockDataChanged(true);
}
