#include "ChunkGenerator.h"
#include "PerlinNoise.h"
#include "Chunk.h"
#include "ChunkCube.h"
#include "Engine/Texture.h"
#include "FastNoise/FastNoise.h"
#include "McWorld.h"
#include "RunnableThread.h"
#include "Blocks/BlockManager.h"

bool ChunkGenerator::bEnableStageDirtGrass = true;
bool ChunkGenerator::bEnableStageCaveCarving = true;
bool ChunkGenerator::bEnableStageTrees = true;
float ChunkGenerator::BaseTerrainHeight = 50;
float ChunkGenerator::TerrainHeightMultiplier = 30;
float ChunkGenerator::TerrainTurbulenceMultiplier = 13;
int ChunkGenerator::HeightMapOctaves = 6;
float ChunkGenerator::HeightMapFrequency = 1 / 600.f;
int ChunkGenerator::TurbulenceOctaves = 2;
float ChunkGenerator::TurbulenceFrequency = 1 / 40.f;
float ChunkGenerator::SlopeGrass = 5;
float ChunkGenerator::SlopeDirt = 7;
int ChunkGenerator::CaveOctaves = 4;
float ChunkGenerator::CaveFrequency = 1 / 400.f;
float ChunkGenerator::CaveThreshold = .035;



uint32 ChunkGenerator::Run()
{
	while (bRun)
	{
		bIsBusy = false;
		Wait();
		if (!bRun)
			break;

		if (bGenerateChunk)
		{
			GenerateChunkData();
			bGenerateChunk = false;
			CurrentChunk = nullptr;
			Pos2D = { -999, -999 };
		}
		if (bGenerateChunkCube)
		{
			GenerateChunkCubes();
			bGenerateChunkCube = false;
		}
		bIsBusy = false;
	}

	
	return 0;
}

B_Block* ChunkGenerator::GetBlockAt(int32 X, int32 Y, int32 Z)
{
	int8 PosZ = static_cast<int8>(floorf(Z / 16));
	
	ChunkCube* c = CurrentChunk->GetChunkCube(PosZ);
	if (c && c->GetBlockData().Num() == 16)
		return c->GetBlockAt(X, Y, Z % 16);

	return nullptr;
}

void ChunkGenerator::SetBlockAt(int32 X, int32 Y, int32 Z, B_Block* Block)
{
	int8 PosZ = static_cast<int8>(floorf(Z / 16));

	if (ChunkCube* c = CurrentChunk->GetChunkCube(PosZ))
		c->GetBlockData()[X][Y][Z % 16] = Block;

}

void ChunkGenerator::GenerateChunkData()
{
	CurrentChunk->SetIsGenerating(true);
	Pos2D = CurrentChunk->GetPos() * 16;
	GenHeightMap();
	World->FinalizeChunkGen(CurrentChunk);
	CurrentChunk->SetIsGenerating(false);
}

void ChunkGenerator::GenerateChunkCubes()
{
	CurrentChunk = CubeGenerationList[0].Cube->GetParentChunk();

	bool bIsGenerating = true;

	for (int stage = 0; bIsGenerating; ++stage)
	{
		for (auto& cube : CubeGenerationList)
		{
			CurrentCubeElement = cube;
			CurrentCubeElement.Cube->SetNextGenerationStage(stage);
			Pos = CurrentCubeElement.Cube->GetPos() * 16;
			switch (stage)
			{
			case 0:
				CurrentCubeElement.Cube->SetIsGenerating(true);
				Stage_BaseStoneTerrain();
				break;
			case 1:
				Stage_DirtGrass();
				break;
			case 2:
				Stage_CaveCarving();
				break;
			case 3:
				Stage_Trees();
				break;
			case 255:
				World->FinalizeCubeGen(CurrentCubeElement.Cube, CurrentCubeElement.Cube->GetPos());
				CurrentCubeElement.Cube->SetIsGenerating(false);
				ChunkMeshGenerator::GenerateChunkMesh(CurrentCubeElement.Cube);
				bIsGenerating = false;
				break;
			default:
				stage = 254;
				break;
			}
		}
	}
}

void ChunkGenerator::GenHeightMap()
{
	ChunkGenMaps& Maps = CurrentChunk->GetChunkGenMaps();

	//Noise setup
		//Height
	UFastNoise* HeightNoise = NewObject<UFastNoise>();
	HeightNoise->SetNoiseType(ENoiseType::SimplexFractal);
	HeightNoise->SetFractalOctaves(HeightMapOctaves);
	HeightNoise->SetFrequency(HeightMapFrequency);
	//Turbulence for height
	UFastNoise* TurbulenceNoise = NewObject<UFastNoise>();
	TurbulenceNoise->SetNoiseType(ENoiseType::SimplexFractal);
	TurbulenceNoise->SetFrequency(TurbulenceFrequency);
	TurbulenceNoise->SetFractalOctaves(TurbulenceOctaves);

	//Height map generation
		//26 x 26 Array (base = 16 x 16; + 5 on each side)
	Maps.HeightMap.SetNum(16);
	for (int x = 0; x < 16; ++x)
	{
		Maps.HeightMap[x].SetNum(16);
		for (int y = 0; y < 16; ++y)
		{
			float xOffset = TurbulenceNoise->GetNoise(Pos2D.X + x + 154, 0) * TerrainTurbulenceMultiplier;
			float yOffset = TurbulenceNoise->GetNoise(0, Pos2D.Y + y + 1615) * TerrainTurbulenceMultiplier;
			Maps.HeightMap[x][y] = HeightNoise->GetNoise(Pos2D.X + x + xOffset, Pos2D.Y + y + yOffset) * TerrainHeightMultiplier + BaseTerrainHeight;
		}
	}

}

void ChunkGenerator::SetChunkData(Chunk* WorkingChunk)
{
	CurrentChunk = WorkingChunk;
	bIsBusy = true;
	bGenerateChunk = true;
}

void ChunkGenerator::SetCubesData(TArray<ChunkCubeGenBufferElement>& Cubes)
{
	bIsBusy = true;
	CubeGenerationList = Cubes;

	bGenerateChunkCube = true;
}

void ChunkGenerator::SetWorld(AMcWorld* NewWorld)
{
	World = NewWorld;
}

void ChunkGenerator::Stage_BaseStoneTerrain()
{
	auto& BlockData = CurrentCubeElement.Cube->GetBlockData();
	ChunkGenMaps& Maps = CurrentChunk->GetChunkGenMaps();

	//Noise setup
		//HeightMap Noise
	UFastNoise* HeightNoise = NewObject<UFastNoise>();
	HeightNoise->SetNoiseType(ENoiseType::SimplexFractal);
	HeightNoise->SetFractalOctaves(HeightMapOctaves);
	HeightNoise->SetFrequency(HeightMapFrequency);
		//Turbulence for height
	UFastNoise * TurbulenceNoise = NewObject<UFastNoise>();
	TurbulenceNoise->SetNoiseType(ENoiseType::SimplexFractal);
	TurbulenceNoise->SetFrequency(TurbulenceFrequency);
	TurbulenceNoise->SetFractalOctaves(TurbulenceOctaves);

	//Terrain generation
	if (BlockData.Num() != 16)
		BlockData.SetNum(16);
	for (int x = 0; x < 16; ++x)
	{
		if (BlockData[x].Num() != 16)
			BlockData[x].SetNum(16);
		for (int y = 0; y < 16; ++y)
		{
			if (BlockData[x][y].Num() != 16)
				BlockData[x][y].SetNum(16);
			

			for (int z = 0; z < 16; ++z)
			{
				int RelX = x + TurbulenceNoise->GetNoise(x + Pos.X, z) * TerrainTurbulenceMultiplier;
				int RelY = y + TurbulenceNoise->GetNoise(y + Pos.Y, z) * TerrainTurbulenceMultiplier;
				float HeightMapValue;
				if (RelX >= 0 && RelX < 16 && RelY >= 0 && RelY < 16)
					HeightMapValue = Maps.HeightMap[RelX][RelY];
				else
					HeightMapValue = HeightNoise->GetNoise(Pos.X + RelX, Pos.Y + RelY) * TerrainHeightMultiplier + BaseTerrainHeight;

				if (Pos.Z + z <= HeightMapValue)
					BlockData[x][y][z] = BlockManager::GetBlock("Stone");
				else
					BlockData[x][y][z] = BlockManager::GetStaticBlock("Air");
			}
		}
	}
}

void ChunkGenerator::Stage_DirtGrass()
{
	if (!bEnableStageDirtGrass)
		return;
	
	auto& BlockData = CurrentCubeElement.Cube->GetBlockData();
	ChunkGenMaps& Maps = CurrentChunk->GetChunkGenMaps();

	for (int x = 0; x < 16; ++x)
	{
		for (int y = 0; y < 16; ++y)
		{
			float slope = abs(Maps.HeightMap[x][y] - Maps.HeightMap[x < 15 ? x + 1 : 14][y < 15 ? y + 1 : 14]);
			for (int z = 0; z < 16; ++z)
			{
				int blocksUp = fmax(1, fmin(5 / slope, 6));
				FName* nextBlocks = new FName[blocksUp];
				for (int zNext = 0; zNext < blocksUp; zNext++)
				{
					if (B_Block* b = GetBlockAt(x, y, z + zNext + Pos.Z))
						nextBlocks[zNext] = b->GetName();
					else
						nextBlocks[zNext] = "Air";
				}
				
				if (nextBlocks[0] == "Stone")
				{
					if (HasAirInRange(nextBlocks, 2, blocksUp))
					{
						if (nextBlocks[1] != "Air")
						{
							if (nextBlocks[0] != "Air")
								delete BlockData[x][y][z];
							BlockData[x][y][z] = BlockManager::GetBlock("Dirt");
						}
						else
						{
							if (nextBlocks[0] != "Air")
								delete BlockData[x][y][z];
							BlockData[x][y][z] = BlockManager::GetBlock("Grass");
						}
					}
				}
				delete [] nextBlocks;
			}
		}
	}
}

void ChunkGenerator::Stage_CaveCarving()
{
	if (!bEnableStageCaveCarving)
		return;
	
	auto& BlockData = CurrentCubeElement.Cube->GetBlockData();
	ChunkGenMaps& Maps = CurrentChunk->GetChunkGenMaps();

	//Cave noise 1
	UFastNoise* CaveNoise1 = NewObject<UFastNoise>();
	CaveNoise1->SetFractalOctaves(CaveOctaves);
	CaveNoise1->SetFrequency(CaveFrequency);
	CaveNoise1->SetNoiseType(ENoiseType::SimplexFractal);

	for (int x = 0; x < 16; ++x)
	{
		for (int y = 0; y < 16; ++y)
		{
			for (int z = 0; z < 16; ++z)
			{
				float bottomFactor = 1.0;
				float topFactor = 1.0;
				if (z < 6)
					bottomFactor = expf(-z + 6.5f) + 1.f;
				if (z > 50)
					topFactor = expf(0.4f * z -23.1) + 1.f;
				float CaveNoiseValue = abs(CaveNoise1->GetNoise(Pos.X + x, Pos.Y + y, z + Pos.Z)) * bottomFactor * topFactor;

				if (CaveNoiseValue < CaveFrequency)
				{
					if (BlockData[x][y][z]->GetName() != "Air")
						delete BlockData[x][y][z];
					BlockData[x][y][z] = BlockManager::GetStaticBlock("Air");
				}
			}
		}
	}
}

void ChunkGenerator::Stage_Trees()
{
	if (!bEnableStageTrees)
		return;
	
	auto& BlockData = CurrentCubeElement.Cube->GetBlockData();
	ChunkGenMaps& Maps = CurrentChunk->GetChunkGenMaps();

	FRandomStream Rand;
	for (int x = 0; x < 16; ++x)
	{
		for (int y = 0; y < 16; ++y)
		{
			int32 seed = (x + Pos.X + y + Pos.Y) ^ ((x + Pos.X) * (y + Pos.Y)) ^ ((x + Pos.X) - (y + Pos.Y));
			Rand.Initialize(seed);
			auto rand = Rand.RandRange(0, 48);
			if (rand == 10)
			{
				for (int z = 15; z >= 0; z--)
				{
					if (BlockData[x][y][z]->GetName() == "Grass")
					{
						int TreeHeight = Rand.RandRange(6, 10);
						for (int zT = z + 1; zT <= z + TreeHeight; ++zT)
						{
							SetBlockAt(x, y, zT + Pos.Z, BlockManager::GetBlock("LogOak"));
						}

						for (int zt = z + TreeHeight - 3; zt <= z + TreeHeight; ++zt)
						{
							for (int xt = x - 2; xt <= x + 2; ++xt)
							{
								for (int yt = y - 2; yt <= y + 2; ++yt)
								{
									if ((pow(xt - x, 2) + pow(yt - y, 2) == 8)
										|| (pow(xt - x, 2) + pow(yt - y, 2) <= 2 && zt == z + TreeHeight - 3
										|| xt == x && yt == y)
										|| Rand.RandRange(0,6) == 5)
										continue;
									if (xt >= 0 && xt < 16 && yt >= 0 && yt < 16)		//Within chunkBoundaries
									{
										if (zt < 16)
											BlockData[xt][yt][zt] = BlockManager::GetBlock("LeavesOak");
										else
											SetBlockAt(xt, yt, zt + Pos.Z, BlockManager::GetBlock("LeavesOak"));
									}
									else
										World->AddBlockSetTask(Pos.X + xt, Pos.Y + yt, Pos.Z + zt, BlockManager::GetBlock("LeavesOak"), 1, false);
								}
							}
						}
						SetBlockAt(x, y, z + TreeHeight + 1 + Pos.Z, BlockManager::GetBlock("LeavesOak"));
						
						break;
					}
				}
			}
		}
	}
}

bool ChunkGenerator::HasAirInRange(FName* NextBlocks, int Start, int End)
{
	for (int i = Start; i < End; i++)
	{
		if (NextBlocks[i] == "Air")
			return true;
	}

	return false;
}

void ChunkGenerator::Wait()
{
	while (!bGenerateChunk && !bGenerateChunkCube && bRun)
	{
		FPlatformProcess::Sleep(.01);	// wait
	}
}

ChunkGenerator::~ChunkGenerator()
{
	if (ThisThread)
		delete ThisThread;
}