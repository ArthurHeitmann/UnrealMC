#include "ChunkCube.h"
#include "../Blocks/B_Block.h"
#include "RuntimeMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "McWorld.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "ChunkMeshGenerator.h"

ChunkCube::ChunkCube(ChunkFormCoords3D Pos, class AMcWorld* McWorld, Chunk* ParentChunk)
{
	this->Pos = Pos;
	this->McWorld = McWorld;
	this->ParentChunk = ParentChunk;
	
	Root = NewObject<USceneComponent>(McWorld);
	Root->SetupAttachment(ParentChunk->Root);
	Root->RegisterComponent();
	Root->AddWorldOffset({ 0.f, 0.f, Pos.Z * 1600.f });
	ChunkMesh = NewObject<URuntimeMeshComponent>(McWorld);
	ChunkMesh->SetupAttachment(Root);
	ChunkMesh->RegisterComponent();
	CustomCollisionMesh = NewObject<URuntimeMeshComponent>(McWorld);
	CustomCollisionMesh->SetupAttachment(Root);
	CustomCollisionMesh->RegisterComponent();

	BoundingBox = NewObject<UBoxComponent>(McWorld);
	BoundingBox->SetupAttachment(Root);
	BoundingBox->RegisterComponent();
	BoundingBox->SetBoxExtent({ 800.f, 800.f, 800.f });
	BoundingBox->AddRelativeLocation({ 800.f, 800.f, 800.f });
	BoundingBox->bHiddenInGame = true;
}

void ChunkCube::Tick(float Delta)
{
	int32 t21 = FDateTime::Now().GetMillisecond();
	if (bHasMeshDataChanged && bHasFinishedGenerating)
		UpdateMesh();
	int32 t22 = FDateTime::Now().GetMillisecond();
}

ChunkCube::~ChunkCube()
{
	//wait for block data to finish generating
	while (bIsGenerating)
		FPlatformProcess::Sleep(0.01);

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

void ChunkCube::UpdateMesh()
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
		ChunkMesh->CreateMeshSection(key, i.Value(), Triangles[key], Normals[key], UVs[key], TArray<FColor>(), TArray<FRuntimeMeshTangent>(), true);
		int32 t42 = FDateTime::Now().GetMillisecond();
		ProcMeshGenTime += t42 - t41;
		ChunkMesh->SetMaterial(key, Materials[key]->GetMaterial(ChunkMesh));
		numFace += Vertices[key].Num() / 4;
	}
	//UE_LOG(LogTemp, Warning, TEXT("ms/face: %f\n"), ((float) t12 - t11) / numFace);


	bHasMeshDataChanged = false;
	MeshLock.Unlock();
}

ChunkFormCoords3D ChunkCube::GetPos()
{
	return Pos;
}

TArray<TArray<TArray<class B_Block*>>>& ChunkCube::GetBlockData()
{
	return BlockData;
}

void ChunkCube::SetNextGenerationStage(int NewStage)
{
	NextGenerationStage = NewStage;
}

int ChunkCube::GetNextGenerationStage()
{
	return NextGenerationStage;
}

Chunk* ChunkCube::GetParentChunk()
{
	return ParentChunk;
}

void ChunkCube::SetParentChunk(Chunk* PChunk)
{
	ParentChunk = PChunk;
}

void ChunkCube::SetHasMeshDataChanged(bool val)
{
	bHasMeshDataChanged = val;
}

void ChunkCube::SetHasBlockDataChanged(bool val)
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

bool ChunkCube::GetHasFinishedGenerating()
{
	return bHasFinishedGenerating;
}

void ChunkCube::SetHasFinishedGenerating(bool val)
{
	bHasFinishedGenerating = val;
}

void ChunkCube::SetIsMeshGenPending(bool val)
{
	bIsMeshGenPending = val;
}

bool ChunkCube::GetIsGenerating()
{
	return bIsGenerating;
}

void ChunkCube::SetIsGenerating(bool val)
{
	bIsGenerating = val;
}

B_Block*& ChunkCube::GetBlockAt(int x, int y, int z)
{
	return BlockData[x][y][z];
}

ChunkCubeGenData& ChunkCube::GetChunkCubeGenData()
{
	return CubeData;
}

ChunkCubeNeighbors& ChunkCube::GetChunkCubeNeighbors()
{
	return CubeNeighbors;
}

void ChunkCube::UpdateCubeNeighbor(EDirection NeighborSide, ChunkCube* NewNeighbor, bool bUpdateMesh)
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
