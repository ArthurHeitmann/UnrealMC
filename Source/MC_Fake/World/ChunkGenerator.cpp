#include "ChunkGenerator.h"
#include "PerlinNoise.h"
#include "Chunk.h"
#include "ChunkCube.h"
#include "Engine/Texture.h"
#include "FastNoise/FastNoise.h"
#include "McWorld.h"


uint32 ChunkGenerator::Run()
{
	while (bRun)
	{
		bIsBusy = false;
		wait();
		if (!bRun)
			break;

		if (bGenerateChunk)
		{
			GenerateChunkData();
			bGenerateChunk = false;
			CurrentChunk = nullptr;//		TODO CR
			Pos2D = { -999, -999 };//
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

B_Block* ChunkGenerator::GetBlockAt(int32 x, int32 y, int32 z)
{
	int8 PosZ = (int8) floorf(z / 16);

	if (ChunkCube* c = CurrentChunk->GetChunkCube(PosZ))
		return c->GetBlockAt(x, y, z % 16);

	return nullptr;
}

void ChunkGenerator::SetBlockAt(int32 x, int32 y, int32 z, B_Block* b)
{
	int8 PosZ = (int8) floorf(z / 16);

	if (ChunkCube* c = CurrentChunk->GetChunkCube(PosZ))
		c->GetBlockData()[x][y][z % 16] = b;

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
				//Stage_CaveCarving();
				break;
			case 3:
				//Stage_Trees();
				break;
			case 255:
				World->FinalizeCubeGen(CurrentCubeElement.Cube, CurrentCubeElement.Cube->GetPos());
				CurrentCubeElement.Cube->SetIsGenerating(false);
				bIsGenerating = false;
				break;
			default:
				stage = 254;
				break;
			}
		}
	}
}

void ChunkGenerator::GenerateChunkCube()
{
	UE_LOG(LogTemp, Error, TEXT("Deprecated Method"));
	Stage_BaseStoneTerrain();
	/*Stage_DirtGrass();
	Stage_CaveCarving();*/
	//Stage_Trees();
}

void ChunkGenerator::GenHeightMap()
{
	ChunkGenMaps& Maps = CurrentChunk->GetChunkGenMaps();

	//Noise setup
		//Height
	UFastNoise* HeightNoise = NewObject<UFastNoise>();
	HeightNoise->SetNoiseType(ENoiseType::SimplexFractal);
	HeightNoise->SetFractalOctaves(6);
	HeightNoise->SetFrequency(1.f / 600.f);
	//Turbulence for height
	UFastNoise* TurbulenceNoise = NewObject<UFastNoise>();
	TurbulenceNoise->SetFrequency(1.f / 40.f);
	TurbulenceNoise->SetNoiseType(ENoiseType::SimplexFractal);
	TurbulenceNoise->SetFractalOctaves(2);

	//Height map generation
		//26 x 26 Array (base = 16 x 16; + 5 on each side)
	Maps.HeightMap.SetNum(16);
	for (int x = 0; x < 16; ++x)
	{
		Maps.HeightMap[x].SetNum(16);
		for (int y = 0; y < 16; ++y)
		{
			float xOffset = TurbulenceNoise->GetNoise(Pos2D.x + x, 0) * 13.f;
			float yOffset = TurbulenceNoise->GetNoise(0, Pos2D.y + y) * 13.f;
			Maps.HeightMap[x][y] = HeightNoise->GetNoise(Pos2D.x + x + xOffset, Pos2D.y + y + yOffset);
		}
	}

}

void ChunkGenerator::SetChunkData(Chunk* WorkingChunk)
{
	CurrentChunk = WorkingChunk;
	bIsBusy = true;
	ChunkFormCoords2D tmp = WorkingChunk->GetPos();

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
	HeightNoise->SetFractalOctaves(6);
	HeightNoise->SetFrequency(1.f / 600.f);
		//Turbulence for height
	UFastNoise * TurbulenceNoise = NewObject<UFastNoise>();
	TurbulenceNoise->SetFrequency(1.f / 40.f);
	TurbulenceNoise->SetNoiseType(ENoiseType::SimplexFractal);
	TurbulenceNoise->SetFractalOctaves(2);

	//Terrain generation
	int WaterLow = 28;
	float minCaveNoise = 0;
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
				int RelX = x + TurbulenceNoise->GetNoise(x + Pos.x, z) * 13.f;
				int RelY = y + TurbulenceNoise->GetNoise(y + Pos.y, z) * 13.f;
				RelX = x;
				RelY = y;
				float HeightMapValue;
				if (RelX >= 0 && RelX < 16 && RelY >= 0 && RelY < 16)
					HeightMapValue = Maps.HeightMap[RelX][RelY];
				else //TODO increase height map seize?
					HeightMapValue = HeightNoise->GetNoise(Pos.x + RelX, Pos.y + RelY);

				HeightMapValue = HeightMapValue * 5.f + 56;
				
				if (Pos.z + z <= HeightMapValue)
					BlockData[x][y][z] = World->GetBlockFromEnum(BStone);
				/*else if (z < 44)
					BlockData[x][y][z] = World->GetBlock(BWater);*/
				else
					BlockData[x][y][z] = World->GetBlockFromEnum(BAir);
			}
		}
	}
}

void ChunkGenerator::Stage_DirtGrass()
{
	auto& BlockData = CurrentCubeElement.Cube->GetBlockData();
	ChunkGenMaps& Maps = CurrentChunk->GetChunkGenMaps();

	for (int x = 0; x < 16; ++x)
	{
		for (int y = 0; y < 16; ++y)
		{
			float slope = 40 * abs(Maps.HeightMap[x][y] - Maps.HeightMap[x < 15 ? x + 1 : 14][y < 15 ? y + 1 : 14]);
			for (int z = 0; z < 16; ++z)
			{
				int blocksUp = fmax(1, fmin(5 / slope, 6));
				EAllBlocks* nextBlocks = new EAllBlocks[blocksUp];
				for (int zNext = 0; zNext < blocksUp; zNext++)
				{
					if (B_Block* b = GetBlockAt(x, y, z + zNext + Pos.z))
						nextBlocks[zNext] = b->GetBlockEnum();
					else
						nextBlocks[zNext] = BAir;
				}
				
				if (nextBlocks[0] == BStone)
				{
					if (hasAirInRange(nextBlocks, 2, blocksUp))
					{
						if (nextBlocks[1] != BAir)
						{
							if (nextBlocks[0] != BAir)
								delete BlockData[x][y][z];
							BlockData[x][y][z] = World->GetBlockFromEnum(BDirt);
						}
						else
						{
							if (nextBlocks[0] != BAir)
								delete BlockData[x][y][z];
							BlockData[x][y][z] = World->GetBlockFromEnum(BGrass);
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
	auto& BlockData = CurrentCubeElement.Cube->GetBlockData();
	ChunkGenMaps& Maps = CurrentChunk->GetChunkGenMaps();

	//Cave noise 1
	UFastNoise* CaveNoise1 = NewObject<UFastNoise>();
	CaveNoise1->SetFractalOctaves(4);
	CaveNoise1->SetFrequency(1.f / 400.f);
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
				float CaveNoiseValue = abs(CaveNoise1->GetNoise(Pos.x + x, Pos.y + y, z + Pos.z)) * bottomFactor * topFactor;

				if (CaveNoiseValue < .035)
				{
					if (BlockData[x][y][z]->GetBlockEnum() != BAir)
						delete BlockData[x][y][z];
					BlockData[x][y][z] = World->GetBlockFromEnum(BAir);
				}
			}
		}
	}
}

void ChunkGenerator::Stage_Trees()
{
	auto& BlockData = CurrentCubeElement.Cube->GetBlockData();
	ChunkGenMaps& Maps = CurrentChunk->GetChunkGenMaps();

	FRandomStream Rand;
	for (int x = 0; x < 16; ++x)
	{
		for (int y = 0; y < 16; ++y)
		{
			int32 seed = (x + Pos.x + y + Pos.y) ^ ((x + Pos.x) * (y + Pos.y)) ^ ((x + Pos.x) - (y + Pos.y));
			Rand.Initialize(seed);
			auto rand = Rand.RandRange(0, 48);
			//if (rand == 10)
			if (/*((x % 15) == 0 || (y % 15) == 0) && */(rand == 10))
			{
				for (int z = 15; z >= 0; z--)
				{
					if (BlockData[x][y][z]->GetBlockEnum() == BGrass)
					{
						int TreeHeight = Rand.RandRange(6, 10);
						for (int zT = z + 1; zT <= z + TreeHeight; ++zT)
						{
							SetBlockAt(x, y, zT + Pos.z, World->GetBlockFromEnum(BLog_Oak));
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
											BlockData[xt][yt][zt] = World->GetBlockFromEnum(BLeaves_Oak);
										else
											SetBlockAt(xt, yt, zt + Pos.z, World->GetBlockFromEnum(BLeaves_Oak));
									}
									else
										World->AddBlockSetTask(Pos.x + xt, Pos.y + yt, Pos.z + zt, World->GetBlockFromEnum(BLeaves_Oak), 1);
								}
							}
						}
						SetBlockAt(x, y, z + TreeHeight + 1 + Pos.z, World->GetBlockFromEnum(BLeaves_Oak));
						
						break;
					}
				}
			}
		}
	}
}

bool ChunkGenerator::hasAirInRange(EAllBlocks* NextBlocks, int start, int end)
{
	for (int i = start; i < end; i++)
	{
		if (NextBlocks[i] == BAir)
			return true;
	}

	return false;
}

void ChunkGenerator::wait()
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