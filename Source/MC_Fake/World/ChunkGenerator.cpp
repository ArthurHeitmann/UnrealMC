#include "ChunkGenerator.h"
#include "PerlinNoise.h"
#include "Chunk.h"
#include "ChunkCube.h"
#include "Engine/Texture.h"
#include "Enums.h"
#include "FastNoise/FastNoise.h"
#include "McWorld.h"


uint32 ChunkGenerator::Run()
{
	while (bRun)
	{
		bIsBusy = false;
		wait();
		bIsBusy = true;

		if (bGenerateChunk)
		{
			GenerateChunkData();
			bGenerateChunk = false;
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

void ChunkGenerator::GenerateChunkData()
{
	GenHeightMap();
	World->FinalizeChunkGen(Chunk);
}

void ChunkGenerator::GenerateChunkCubes()
{
	while (CubeGenerationList.Num())
	{
		CurrentCubeElement = CubeGenerationList[0];
		GenerateChunkCube();
		World->FinalizeCubeGen(CurrentCubeElement.Cube, CurrentCubeElement.Cube->GetPos());
		CubeGenerationList.RemoveAtSwap(0);
	}
}

void ChunkGenerator::GenerateChunkCube()
{
	Stage_BaseStoneTerrain();
	Stage_DirtGrass();

}

void ChunkGenerator::GenHeightMap()
{
	ChunkGenMaps& Maps = Chunk->GetChunkGenMaps();

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
	for (int x = 0; x < 16; x++)
	{
		Maps.HeightMap[x].SetNum(16);
		for (int y = 0; y < 16; y++)
		{
			float xOffset = TurbulenceNoise->GetNoise(x, 0) * 13.f;
			float yOffset = TurbulenceNoise->GetNoise(0, y) * 13.f;
			Maps.HeightMap[x][y] = HeightNoise->GetNoise(PosX + x + xOffset, PosY + y + yOffset);
		}
	}

}

void ChunkGenerator::SetChunkData(int x, int y, AChunk* WorkingChunk)
{
	PosX = x;
	PosY = y;
	Chunk = WorkingChunk;

	bGenerateChunk = true;
}

void ChunkGenerator::SetCubesData(TArray<ChunkCubeGenBufferElement>& Cubes)
{
	CubeGenerationList = Cubes;

	bGenerateChunkCube = true;
}

//void ChunkGenerator::Reset(int x, int y, AChunk * WorkingChunk, int NewGenerationStage, int MaxGenStage, bool bUseThread)
//{
//	if (bUseThread)
//		bIsDone = false;
//}

void ChunkGenerator::SetWorld(AMcWorld* NewWorld)
{
	World = NewWorld;
}


//void ChunkGenerator::Generate()
//{
//	bool HasDataBeenChanged = false;
//	ChunkBlockData = Chunk->GetChunkBlockData();
//	int32 ChunkX = PosX / 16;
//	int32 ChunkY = PosY / 16;
//
//	while (NextGenerationStage <= LastGenerationStage)
//	{
//		switch (NextGenerationStage)
//		{
//		case 0:
//			Stage_BaseStoneTerrain();
//			HasDataBeenChanged = true;
//			break;
//		case 1:
//			Stage_DirtGrass();
//			HasDataBeenChanged = true;
//			break;
//		case 2:
//			Stage_CaveCarving();
//			HasDataBeenChanged = true;
//			break;
//		case 5:
//			Stage_Trees();
//			HasDataBeenChanged = true;
//			break;
//		case 6:
//			NextGenerationStage = 244;
//			Chunk->SetHasFinishedGenerating(true);
//			break;
//		}
//		Chunk->SetNextGenerationStage(++NextGenerationStage);
//		World->CompleteBlockSetTasks(Chunk, ChunkX, ChunkY);
//
//	}
//
//	if (HasDataBeenChanged)
//	{
//		Chunk->SetHasDataChanged();
//		Chunk->SetNextGenerationStage(255);
//	}
//	World->CompleteBlockSetTasks(Chunk, PosX / 16, PosY / 16);
//	World->FinalizeCubeGen(Chunk, ChunkX, ChunkY);
//	bIsDone = true;
//}

void ChunkGenerator::Stage_BaseStoneTerrain()
{
	auto& BlockData = CurrentCubeElement.Cube->GetBlockData();
	ChunkGenMaps& Maps = Chunk->GetChunkGenMaps();

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
	for (int x = 0; x < 16; x++)
	{
		if (BlockData[x].Num() != 16)
			BlockData[x].SetNum(16);
		for (int y = 0; y < 16; y++)
		{
			if (BlockData[x][y].Num() != 16)
				BlockData[x][y].SetNum(16);
			

			for (int z = 0; z < 16; z++)
			{
				int RelX = x + TurbulenceNoise->GetNoise(x + PosX, z) * 13.f;
				int RelY = y + TurbulenceNoise->GetNoise(y + PosY, z) * 13.f;
				float HeightMapValue;
				if (RelX >= 0 && RelX < 16 && RelY >= 0 && RelY < 16)
					HeightMapValue = Maps.HeightMap[RelX][RelY];
				else //TODO increase height map seize?
					HeightMapValue = HeightNoise->GetNoise(PosX + RelX, PosY + RelY);

				HeightMapValue = HeightMapValue * 40.f + 50;
				
				if (z <= HeightMapValue)
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
	ChunkGenMaps& Maps = Chunk->GetChunkGenMaps();

	for (int x = 0; x < 16; x++)
	{
		for (int y = 0; y < 16; y++)
		{
			for (int z = 0; z < 16; z++)
			{
				EAllBlocks b = BlockData[x][y][z]->GetBlockEnum();
				EAllBlocks b1 = z < 15 ? BlockData[x][y][z + 1]->GetBlockEnum() : BAir;
				EAllBlocks b2 = z < 14 ? BlockData[x][y][z + 2]->GetBlockEnum() : BAir;
				EAllBlocks b3 = z < 13 ? BlockData[x][y][z + 3]->GetBlockEnum() : BAir;
				EAllBlocks b4 = z < 12 ? BlockData[x][y][z + 4]->GetBlockEnum() : BAir;
				EAllBlocks b5 = z < 11 ? BlockData[x][y][z + 5]->GetBlockEnum() : BAir;
				EAllBlocks b6 = z < 10 ? BlockData[x][y][z + 6]->GetBlockEnum() : BAir;
				
				if (b == BStone && (b2 == BAir || b3 == BAir || b4 == BAir || b5 == BAir || b6 == BAir))
				{
					if (b1 != BAir)
					{
						if (b != BAir)
							delete BlockData[x][y][z];
						BlockData[x][y][z] = World->GetBlockFromEnum(BDirt);
					}
					else
					{
						if (b != BAir)
							delete BlockData[x][y][z];
						BlockData[x][y][z] = World->GetBlockFromEnum(BGrass);
					}
				}
			}
		}
	}
}

void ChunkGenerator::Stage_CaveCarving()
{
	auto& BlockData = CurrentCubeElement.Cube->GetBlockData();
	ChunkGenMaps& Maps = Chunk->GetChunkGenMaps();

	//Cave noise 1
	UFastNoise* CaveNoise1 = NewObject<UFastNoise>();
	CaveNoise1->SetFractalOctaves(4);
	CaveNoise1->SetFrequency(1.f / 400.f);
	CaveNoise1->SetNoiseType(ENoiseType::SimplexFractal);

	for (int x = 0; x < 16; x++)
	{
		for (int y = 0; y < 16; y++)
		{
			for (int z = 0; z < 16; z++)
			{
				float CaveNoiseValue = abs(CaveNoise1->GetNoise(PosX + x, PosY + y, z));

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
	ChunkGenMaps& Maps = Chunk->GetChunkGenMaps();

	FRandomStream Rand;
	for (int x = 0; x < 16; x++)
	{
		for (int y = 0; y < 16; y++)
		{
			int32 seed = (x + PosX + y + PosY) ^ ((x + PosX) * (y + PosY)) ^ ((x + PosX) - (y + PosY));
			Rand.Initialize(seed);
			auto rand = Rand.RandRange(0, 48);
			//if (rand == 10)
			if (/*((x % 15) == 0 || (y % 15) == 0) && */(rand == 10))
			{
				for (int z = 200; z > 0; z--)
				{
					if (BlockData[x][y][z]->GetBlockEnum() == BGrass)
					{
						int TreeHeight = Rand.RandRange(6, 10);
						for (int zT = z + 1; zT <= z + TreeHeight; zT++)
						{
							BlockData[x][y][zT] = World->GetBlockFromEnum(BLog_Oak);
						}

						for (int zt = z + TreeHeight - 3; zt <= z + TreeHeight; zt++)
						{
							for (int xt = x - 2; xt <= x + 2; xt++)
							{
								for (int yt = y - 2; yt <= y + 2; yt++)
								{
									if ((pow(xt - x, 2) + pow(yt - y, 2) == 8)
										|| (pow(xt - x, 2) + pow(yt - y, 2) <= 2 && zt == z + TreeHeight - 3
										|| xt == x && yt == y)
										|| Rand.RandRange(0,6) == 5)
										continue;
									if (xt > -1 && xt < 16 && yt > -1 && yt < 16)			//Within chunkBoundaries
										BlockData[xt][yt][zt] = World->GetBlockFromEnum(BLeaves_Oak);
									else
										World->AddBlockSetTask(PosX + xt, PosY + yt, zt, World->GetBlockFromEnum(BLeaves_Oak), 1);
								}
							}
						}
						BlockData[x][y][z + TreeHeight + 1] = World->GetBlockFromEnum(BLeaves_Oak);
						
						break;
					}
				}
			}
		}
	}
}

void ChunkGenerator::wait()
{
	while (!bGenerateChunk && !bGenerateChunkCube)
	{
		FPlatformProcess::Sleep(.05);	// wait
	}
}

ChunkGenerator::~ChunkGenerator()
{
	if (ThisThread)
		delete ThisThread;
}