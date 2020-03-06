#include "ChunkMeshGenerator.h"
#include "ChunkCube.h"

void ChunkMeshGenerator::GenerateChunkMesh(ChunkCube* C)
{
	int32 t11 = FDateTime::Now().GetMillisecond();
	C->MeshLock.Lock();
	int32 t1D = FDateTime::Now().GetMillisecond() - t11;
	if (t1D > 0)
		UE_LOG(LogTemp, Warning, TEXT("MeshGen Locking for %d"), t1D);

	//UE_LOG(LogTemp, Warning, TEXT("Generating Mesh for %d %d %d"), C->GetPos().X, C->GetPos().Y, C->GetPos().Z);

	C->SetHasMeshDataChanged();
	C->SetHasBlockDataChanged(false);
	
	
	auto& BlockData = C->GetBlockData();
	auto& Vertices = C->Vertices;
	auto& Triangles = C->Triangles;
	auto& UVs = C->UVs;
	auto& Normals = C->Normals;
	auto& Materials = C->Materials;
	auto& CubeNeighbors = C->GetChunkCubeNeighbors();

	Vertices.Empty();
	Triangles.Empty();
	UVs.Empty();
	Normals.Empty();
	Materials.Empty();
	
	
	for (int x = 0; x < 16; ++x) {
		for (int y = 0; y < 16; ++y)
		{
			for (int z = 0; z < 16; ++z)
			{
				if (BlockData[x][y][z]->GetBlockModelType() != EBlockModelType::NONE)
				{
					uint16 BlockID = BlockData[x][y][z]->GetBlockID();
					//If necessary create keys for new block type in all maps
					if (!Materials.Contains(BlockID))
					{
						Materials.Add(BlockID, BlockData[x][y][z]);
						Vertices.Add(BlockID);
						UVs.Add(BlockID);
						Triangles.Add(BlockID);
						Normals.Add(BlockID);
						//TODO Calculate estimates based on BlockID, Pos.Z, etc.
						Vertices[BlockID].Reserve(1000);
						UVs[BlockID].Reserve(1000);
						Triangles[BlockID].Reserve(1000);
						Normals[BlockID].Reserve(1000);
					}
					//Handle custom block models
					if (BlockData[x][y][z]->UsesCustomModel()) {
						Triangles[BlockID].Append(BlockData[x][y][z]->GetAllTrianglesFrom(Vertices[BlockID].Num()));
						Vertices[BlockID].Append(BlockData[x][y][z]->GetAllVertices(x * 100, y * 100, z * 100));
						UVs[BlockID].Append(BlockData[x][y][z]->GetAllUVs());
						Normals[BlockID].Append(BlockData[x][y][z]->GetAllNormals());
						continue;
					}
					//Top
					if (z != 15 && ShouldFaceBePlacedBetween(BlockData[x][y][z], BlockData[x][y][z + 1], TOP)
						||
						z == 15 && CubeNeighbors.Top && ShouldFaceBePlacedBetween(BlockData[x][y][z], CubeNeighbors.Top->GetBlockAt(x, y, 0), TOP))
					{
						Triangles[BlockID].Append(BlockData[x][y][z]->GetTopTrianglesFrom(Vertices[BlockID].Num()));
						Vertices[BlockID].Append(BlockData[x][y][z]->GetTopVertices(x * 100, y * 100, z * 100));
						UVs[BlockID].Append(BlockData[x][y][z]->GetTopUVs());
						Normals[BlockID].Append(BlockData[x][y][z]->GetTopNormals());
					}
					//Bottom
					if (z != 0 && ShouldFaceBePlacedBetween(BlockData[x][y][z], BlockData[x][y][z - 1], BOTTOM)
						||
						z == 0 && CubeNeighbors.Bottom && ShouldFaceBePlacedBetween(BlockData[x][y][z], CubeNeighbors.Bottom->GetBlockAt(x, y, 15), BOTTOM))
					{
						Triangles[BlockID].Append(BlockData[x][y][z]->GetBottomTrianglesFrom(Vertices[BlockID].Num()));
						Vertices[BlockID].Append(BlockData[x][y][z]->GetBottomVertices(x * 100, y * 100, z * 100));
						UVs[BlockID].Append(BlockData[x][y][z]->GetBottomUVs());
						Normals[BlockID].Append(BlockData[x][y][z]->GetBottomNormals());
					}
					//Right
					if (y != 15 && ShouldFaceBePlacedBetween(BlockData[x][y][z], BlockData[x][y + 1][z], EAST)
						||
						y == 15 && CubeNeighbors.East && ShouldFaceBePlacedBetween(BlockData[x][y][z], CubeNeighbors.East->GetBlockAt(x, 0, z), EAST))
					{
						Triangles[BlockID].Append(BlockData[x][y][z]->GetRightTrianglesFrom(Vertices[BlockID].Num()));
						Vertices[BlockID].Append(BlockData[x][y][z]->GetRightVertices(x * 100, y * 100, z * 100));
						UVs[BlockID].Append(BlockData[x][y][z]->GetRightUVs());
						Normals[BlockID].Append(BlockData[x][y][z]->GetRightNormals());
					}
					//Left
					if (y && ShouldFaceBePlacedBetween(BlockData[x][y][z], BlockData[x][y - 1][z], WEST)
						||
						y == 0 && CubeNeighbors.West && ShouldFaceBePlacedBetween(BlockData[x][y][z], CubeNeighbors.West->GetBlockAt(x, 15, z), WEST))
					{
						Triangles[BlockID].Append(BlockData[x][y][z]->GetLeftTrianglesFrom(Vertices[BlockID].Num()));
						Vertices[BlockID].Append(BlockData[x][y][z]->GetLeftVertices(x * 100, y * 100, z * 100));
						UVs[BlockID].Append(BlockData[x][y][z]->GetLeftUVs());
						Normals[BlockID].Append(BlockData[x][y][z]->GetLeftNormals());
					}
					//Front
					if (x && ShouldFaceBePlacedBetween(BlockData[x][y][z], BlockData[x - 1][y][z], SOUTH)
						||
						x == 0 && CubeNeighbors.South && ShouldFaceBePlacedBetween(BlockData[x][y][z], CubeNeighbors.South->GetBlockAt(15, y, z), SOUTH))
					{
						Triangles[BlockID].Append(BlockData[x][y][z]->GetFrontTrianglesFrom(Vertices[BlockID].Num()));
						Vertices[BlockID].Append(BlockData[x][y][z]->GetFrontVertices(x * 100, y * 100, z * 100));
						UVs[BlockID].Append(BlockData[x][y][z]->GetFrontUVs());
						Normals[BlockID].Append(BlockData[x][y][z]->GetFrontNormals());
					}
					//Back
					if (x != 15 && ShouldFaceBePlacedBetween(BlockData[x][y][z], BlockData[x + 1][y][z], NORTH)
						||
						x == 15 && CubeNeighbors.North && ShouldFaceBePlacedBetween(BlockData[x][y][z], CubeNeighbors.North->GetBlockAt(0, y, z), NORTH))
					{
						Triangles[BlockID].Append(BlockData[x][y][z]->GetBackTrianglesFrom(Vertices[BlockID].Num()));
						Vertices[BlockID].Append(BlockData[x][y][z]->GetBackVertices(x * 100, y * 100, z * 100));
						UVs[BlockID].Append(BlockData[x][y][z]->GetBackUVs());
						Normals[BlockID].Append(BlockData[x][y][z]->GetBackNormals());
					}
				}
			}
		}
	}

	C->MeshLock.Unlock();
}

ChunkMeshGeneratorThread::ChunkMeshGeneratorThread()
{
	
	thisThread = FRunnableThread::Create(this, TEXT("Chunk Mesh Gen Thread"));
}

void ChunkMeshGeneratorThread::SetChunkCube(ChunkCube* C)
{
	Cube = C;
	bNewTask = true;
	bIsBusy = true;
}

void ChunkMeshGeneratorThread::SetWorld(AMcWorld* W)
{
	World = W;
}

uint32 ChunkMeshGeneratorThread::Run()
{
	while (bRun)
	{
		while (!bNewTask)
		{
			FPlatformProcess::Sleep(0.005);
			if (!bRun)
				return 0;
		}
			
		bNewTask = false;
		Cube->SetIsMeshGenPending(false);
		ChunkMeshGenerator::GenerateChunkMesh(Cube);
		if (World)
			World->DequeueMeshGenTasks();
		bIsBusy = false;
	}
	return 0;
}

bool ChunkMeshGenerator::ShouldFaceBePlacedBetween(B_Block* b1, B_Block* b2, EDirection Side)
{
	return b2->IsSideOptimizable(Side)
		&& b2->GetBlockModelType() != BLOCK
		|| !b2->IsBlockOpaque();
}
