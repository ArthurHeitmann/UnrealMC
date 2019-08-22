#include "ChunkGenerator.h"
#include "PerlinNoise.h"
#include "McWorld.h"
#include "Chunk.h"
#include "Engine/Texture.h"
#include "Enums.h"
#include "FastNoise/FastNoise.h"


ChunkGenerator::~ChunkGenerator()
{
	if (bUsesThread)
		delete ThisThread;
}

uint32 ChunkGenerator::Run()
{
	while (bRun)
	{
		wait();
		Generate();
	}


	return 0;
}

void ChunkGenerator::Reset(int x, int y, AChunk * WorkingChunk, int NewGenerationStage, int MaxGenStage, bool bUseThread)
{
	PosX = x;
	PosY = y;
	Chunk = WorkingChunk;
	NextGenerationStage = NewGenerationStage;
	LastGenerationStage = MaxGenStage;
	bUsesThread = bUseThread;
	if (bUseThread)
		bIsDone = false;
}

void ChunkGenerator::SetWorld(AMcWorld* NewWorld)
{
	World = NewWorld;
}


void ChunkGenerator::Generate()
{
	bool HasDataBeenChanged = false;
	ChunkBlockData = Chunk->GetChunkBlockData();
	int32 ChunkX = PosX / 16;
	int32 ChunkY = PosY / 16;

	while (NextGenerationStage <= LastGenerationStage)
	{
		switch (NextGenerationStage)
		{
		case 0:
			Stage0();
			HasDataBeenChanged = true;
			break;
		case 1:
			Stage1();
			HasDataBeenChanged = true;
			break;
		case 2:
			Stage2();
			HasDataBeenChanged = true;
			break;
		case 5:
			Stage5();
			HasDataBeenChanged = true;
			break;
		case 6:
			NextGenerationStage = 244;
			Chunk->SetHasFinishedGenerating(true);
			break;
		}
		Chunk->SetNextGenerationStage(++NextGenerationStage);
		World->CompleteBlockSetTasks(Chunk, ChunkX, ChunkY);

	}

	if (HasDataBeenChanged)
	{
		Chunk->SetHasDataChanged();
		Chunk->SetNextGenerationStage(255);
	}
	World->CompleteBlockSetTasks(Chunk, PosX / 16, PosY / 16);
	World->FinalizeChunkGen(Chunk, ChunkX, ChunkY);
	bIsDone = true;
}

void ChunkGenerator::Stage0()
{
	//Noise setup
		//Height
	UFastNoise* HeightNoise = NewObject<UFastNoise>();
	HeightNoise->SetNoiseType(ENoiseType::SimplexFractal);
	HeightNoise->SetFractalOctaves(6);
	HeightNoise->SetFrequency(1.f / 600.f);
		//Turbulence for height
	UFastNoise * TurbulenceNoise = NewObject<UFastNoise>();
	TurbulenceNoise->SetFrequency(1.f / 40.f);
	TurbulenceNoise->SetNoiseType(ENoiseType::SimplexFractal);
	TurbulenceNoise->SetFractalOctaves(2);
		//Ocean/Lake noise
	UFastNoise* WaterNoise = NewObject<UFastNoise>();
	WaterNoise->SetFractalOctaves(3);
	WaterNoise->SetFrequency(1.f / 1500.f);
	WaterNoise->SetNoiseType(ENoiseType::SimplexFractal);

	//Height map generation
		//26 x 26 Array (base = 16 x 16; + 5 on each side)
	TArray<TArray<float>> HeightMap;
	HeightMap.SetNum(26);
	for (int x = 0; x < 26; x++)
	{
		HeightMap[x].SetNumZeroed(26);
		for (int y = 0; y < 26; y++)
			HeightMap[x][y] = HeightNoise->GetNoise(PosX + x - 5, PosY + y - 5);
	}

	//Terrain generation
	int WaterLow = 28;
	float minCaveNoise = 0;
	if (ChunkBlockData->Num() != 16)
		ChunkBlockData->SetNum(16);
	for (int x = 0; x < 16; x++)
	{
		if ((*ChunkBlockData)[x].Num() != 16)
			(*ChunkBlockData)[x].SetNum(16);
		for (int y = 0; y < 16; y++)
		{
			if ((*ChunkBlockData)[x][y].Num() != 256)
				(*ChunkBlockData)[x][y].SetNum(256);
			
			float wNoise = WaterNoise->GetNoise(x + PosX, y + PosY) * 3;

			for (int z = 0; z < 256; z++)
			{
				int RelX = x + TurbulenceNoise->GetNoise(x + PosX, z) * 13.f;
				int RelY = y + TurbulenceNoise->GetNoise(y + PosY, z) * 13.f;
				float HeightMapValue;
				if (RelX >= -5 && RelX <= 20 && RelY >= -5 && RelY <= 20)
					HeightMapValue = HeightMap[RelX + 5][RelY + 5];
				else
					HeightMapValue = HeightNoise->GetNoise(PosX + RelX, PosY + RelY);

				HeightMapValue = HeightMapValue * 40.f + 50;

				if (wNoise < -.5f)
				{
					float factor = FMath::Clamp<float>(wNoise * (-2.f) - 1, 0.f, 1.f);
					HeightMapValue -= factor * (HeightMapValue - WaterLow);
				}

				
				if (z <= HeightMapValue)
					(*ChunkBlockData)[x][y][z] = World->GetBlock(TEnumAsByte<EAllBlocks>(BStone));
				/*else if (z < 44)
					(*ChunkBlockData)[x][y][z] = World->GetBlock(TEnumAsByte<EAllBlocks>(BWater));*/
				else
					(*ChunkBlockData)[x][y][z] = World->GetBlock(TEnumAsByte<EAllBlocks>(BAir));
			}
		}
	}
}

void ChunkGenerator::Stage1()
{
	for (int x = 0; x < 16; x++)
	{
		for (int y = 0; y < 16; y++)
		{
			for (int z = 0; z < 256; z++)
			{
				EAllBlocks b = (*ChunkBlockData)[x][y][z]->GetBlockEnum();
				EAllBlocks b1 = z < 255 ? (*ChunkBlockData)[x][y][z + 1]->GetBlockEnum() : BAir;
				EAllBlocks b2 = z < 254 ? (*ChunkBlockData)[x][y][z + 2]->GetBlockEnum() : BAir;
				EAllBlocks b3 = z < 253 ? (*ChunkBlockData)[x][y][z + 3]->GetBlockEnum() : BAir;
				EAllBlocks b4 = z < 252 ? (*ChunkBlockData)[x][y][z + 4]->GetBlockEnum() : BAir;
				EAllBlocks b5 = z < 251 ? (*ChunkBlockData)[x][y][z + 5]->GetBlockEnum() : BAir;
				EAllBlocks b6 = z < 250 ? (*ChunkBlockData)[x][y][z + 6]->GetBlockEnum() : BAir;
				
				if (b == BStone && (b2 == BAir || b3 == BAir || b4 == BAir || b5 == BAir || b6 == BAir))
				{
					if (b1 != BAir)
					{
						if (b != BAir)
							delete (*ChunkBlockData)[x][y][z];
						(*ChunkBlockData)[x][y][z] = World->GetBlock(BDirt);
					}
					else
					{
						if (b != BAir)
							delete (*ChunkBlockData)[x][y][z];
						(*ChunkBlockData)[x][y][z] = World->GetBlock(BGrass);
					}
				}
			}
		}
	}
}

void ChunkGenerator::Stage2()
{
	//Cave noise 1
	UFastNoise* CaveNoise1 = NewObject<UFastNoise>();
	CaveNoise1->SetFractalOctaves(4);
	CaveNoise1->SetFrequency(1.f / 400.f);
	CaveNoise1->SetNoiseType(ENoiseType::SimplexFractal);

	for (int x = 0; x < 16; x++)
	{
		for (int y = 0; y < 16; y++)
		{
			for (int z = 0; z < 256; z++)
			{
				float CaveNoiseValue = abs(CaveNoise1->GetNoise(PosX + x, PosY + y, z));

				if (CaveNoiseValue < .035)
				{
					if ((*ChunkBlockData)[x][y][z]->GetBlockEnum() != BAir)
						delete (*ChunkBlockData)[x][y][z];
					(*ChunkBlockData)[x][y][z] = World->GetBlock(TEnumAsByte<EAllBlocks>(BAir));
				}
			}
		}
	}
}

void ChunkGenerator::Stage3()
{
}

void ChunkGenerator::Stage4()
{
}

void ChunkGenerator::Stage5()
{
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
					if ((*ChunkBlockData)[x][y][z]->GetBlockEnum() == BGrass)
					{
						int TreeHeight = Rand.RandRange(6, 10);
						for (int zT = z + 1; zT <= z + TreeHeight; zT++)
						{
							(*ChunkBlockData)[x][y][zT] = World->GetBlock(BLog_Oak);
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
										(*ChunkBlockData)[xt][yt][zt] = World->GetBlock(BLeaves_Oak);
									else
										World->AddBlockSetTask(PosX + xt, PosY + yt, zt, World->GetBlock(BLeaves_Oak), 1);
								}
							}
						}
						(*ChunkBlockData)[x][y][z + TreeHeight + 1] = World->GetBlock(BLeaves_Oak);
						
						break;
					}
				}
			}
		}
	}
}

void ChunkGenerator::Stage6()
{
}

void ChunkGenerator::wait()
{
	while (bIsDone)
	{
		FPlatformProcess::Sleep(.1);	// wait
	}
}
