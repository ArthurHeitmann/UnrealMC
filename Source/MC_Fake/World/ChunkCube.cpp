#include "ChunkCube.h"
#include "../Blocks/B_Block.h"
#include "RuntimeMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "McWorld.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

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
	BoundingBox->bHiddenInGame = false;
}

void ChunkCube::Tick(float Delta)
{
	if (bHasDataChanged && bHasFinishedGenerating)
		UpdateMesh();
}

ChunkCube::~ChunkCube()
{
	while (bIsGenerating)
		FPlatformProcess::Sleep(0.01);

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

	TMap<uint16, TArray<FVector>> Vertices;
	TMap<uint16, TArray<FVector2D>> UVs;
	TMap<uint16, TArray<int32>> Triangles;
	TMap<uint16, TArray<FVector>> Normals;
	TMap<uint16, B_Block*> Materials;
	TMap<uint16, TArray<FVector>> VerticesCustomCollision;
	TMap<uint16, TArray<int32>> TrianglesCustomCollision;
	for (int x = 0; x < 16; ++x)
	{
		for (int y = 0; y < 16; ++y)
		{
			for (int z = 0; z < 16; ++z)
			{
				if (BlockData[x][y][z]->GetBlockModelType() != EBlockModelType::NONE)
				{
					uint16 cbe = BlockData[x][y][z]->GetBlockID(); //current block enum (of this iteration)
					//If necessary create keys for new block type in all maps
					if (!Materials.Contains(cbe))
					{
						Materials.Add(cbe, BlockData[x][y][z]);
						Vertices.Add(cbe);
						UVs.Add(cbe);
						Triangles.Add(cbe);
						Normals.Add(cbe);
					}
					//Handle custom block models
					if (BlockData[x][y][z]->UsesCustomModel()) {
						Triangles[cbe].Append(BlockData[x][y][z]->GetAllTrianglesFrom(Vertices[cbe].Num()));
						Vertices[cbe].Append(BlockData[x][y][z]->GetAllVertices(x * 100, y * 100, z * 100));
						UVs[cbe].Append(BlockData[x][y][z]->GetAllUVs());
						Normals[cbe].Append(BlockData[x][y][z]->GetAllNormals());
						continue;
					}
					//Top
					if (z != 15 && ShouldFaceBePlacedBetween(BlockData[x][y][z], BlockData[x][y][z + 1], TOP)
						||
						z == 15 && CubeNeighbors.Top && ShouldFaceBePlacedBetween(BlockData[x][y][z], CubeNeighbors.Top->GetBlockAt(x, y, 0), TOP))
					{
						Triangles[cbe].Append(BlockData[x][y][z]->GetTopTrianglesFrom(Vertices[cbe].Num()));
						Vertices[cbe].Append(BlockData[x][y][z]->GetTopVertices(x * 100, y * 100, z * 100));
						UVs[cbe].Append(BlockData[x][y][z]->GetTopUVs());
						Normals[cbe].Append(BlockData[x][y][z]->GetTopNormals());
					}
					//Bottom
					if (z != 0 && ShouldFaceBePlacedBetween(BlockData[x][y][z], BlockData[x][y][z - 1], BOTTOM)
						||
						z == 0 && CubeNeighbors.Bottom && ShouldFaceBePlacedBetween(BlockData[x][y][z], CubeNeighbors.Bottom->GetBlockAt(x, y, 15), BOTTOM))
					{
						Triangles[cbe].Append(BlockData[x][y][z]->GetBottomTrianglesFrom(Vertices[cbe].Num()));
						Vertices[cbe].Append(BlockData[x][y][z]->GetBottomVertices(x * 100, y * 100, z * 100));
						UVs[cbe].Append(BlockData[x][y][z]->GetBottomUVs());
						Normals[cbe].Append(BlockData[x][y][z]->GetBottomNormals());
					}
					//Right
					if (y != 15 && ShouldFaceBePlacedBetween(BlockData[x][y][z], BlockData[x][y + 1][z], EAST)
						||
						y == 15 && CubeNeighbors.East && ShouldFaceBePlacedBetween(BlockData[x][y][z], CubeNeighbors.East->GetBlockAt(x, 0, z), EAST))
					{
						Triangles[cbe].Append(BlockData[x][y][z]->GetRightTrianglesFrom(Vertices[cbe].Num()));
						Vertices[cbe].Append(BlockData[x][y][z]->GetRightVertices(x * 100, y * 100, z * 100));
						UVs[cbe].Append(BlockData[x][y][z]->GetRightUVs());
						Normals[cbe].Append(BlockData[x][y][z]->GetRightNormals());
					}
					//Left
					if (y && ShouldFaceBePlacedBetween(BlockData[x][y][z], BlockData[x][y - 1][z], WEST)
						||
						y == 0 && CubeNeighbors.West && ShouldFaceBePlacedBetween(BlockData[x][y][z], CubeNeighbors.West->GetBlockAt(x, 15, z), WEST))
					{
						Triangles[cbe].Append(BlockData[x][y][z]->GetLeftTrianglesFrom(Vertices[cbe].Num()));
						Vertices[cbe].Append(BlockData[x][y][z]->GetLeftVertices(x * 100, y * 100, z * 100));
						UVs[cbe].Append(BlockData[x][y][z]->GetLeftUVs());
						Normals[cbe].Append(BlockData[x][y][z]->GetLeftNormals());
					}
					//Front
					if (x && ShouldFaceBePlacedBetween(BlockData[x][y][z], BlockData[x - 1][y][z], SOUTH)
						||
						x == 0 && CubeNeighbors.South && ShouldFaceBePlacedBetween(BlockData[x][y][z], CubeNeighbors.South->GetBlockAt(15, y, z), SOUTH))
					{
						Triangles[cbe].Append(BlockData[x][y][z]->GetFrontTrianglesFrom(Vertices[cbe].Num()));
						Vertices[cbe].Append(BlockData[x][y][z]->GetFrontVertices(x * 100, y * 100, z * 100));
						UVs[cbe].Append(BlockData[x][y][z]->GetFrontUVs());
						Normals[cbe].Append(BlockData[x][y][z]->GetFrontNormals());
					}
					//Back
					if (x != 15 && ShouldFaceBePlacedBetween(BlockData[x][y][z], BlockData[x + 1][y][z], NORTH)
						||
						x == 15 && CubeNeighbors.North && ShouldFaceBePlacedBetween(BlockData[x][y][z], CubeNeighbors.North->GetBlockAt(0, y, z), NORTH))
					{
						Triangles[cbe].Append(BlockData[x][y][z]->GetBackTrianglesFrom(Vertices[cbe].Num()));
						Vertices[cbe].Append(BlockData[x][y][z]->GetBackVertices(x * 100, y * 100, z * 100));
						UVs[cbe].Append(BlockData[x][y][z]->GetBackUVs());
						Normals[cbe].Append(BlockData[x][y][z]->GetBackNormals());
					}
				}
			}
		}
	}
	
	for (auto i = Vertices.CreateConstIterator(); i; ++i)
	{
		uint16 key = i.Key();
		ChunkMesh->CreateMeshSection(key, i.Value(), Triangles[key], Normals[key], UVs[key], TArray<FColor>(), TArray<FRuntimeMeshTangent>(), true);
		ChunkMesh->SetMaterial(key, Materials[key]->GetMaterial(ChunkMesh));
	}


	bHasDataChanged = false;
}

bool ChunkCube::ShouldFaceBePlacedBetween(B_Block* b1, B_Block* b2, EDirection Side)
{
	return b2->IsSideOptimizable(Side)
		&& b2->GetBlockModelType() != BLOCK
		|| !b2->IsBlockOpaque();
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

void ChunkCube::SetHasDataChanged(bool val)
{
	bHasDataChanged = val;
}

bool ChunkCube::GetHasFinishedGenerating()
{
	return bHasFinishedGenerating;
}

void ChunkCube::SetHasFinishedGenerating(bool val)
{
	bHasFinishedGenerating = val;
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
		bHasDataChanged = true;
}
