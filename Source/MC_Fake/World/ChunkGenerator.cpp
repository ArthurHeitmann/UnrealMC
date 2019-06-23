#include "ChunkGenerator.h"
#include "PerlinNoise.h"
#include "McWorld.h"
#include "Chunk.h"
#include "Engine/Texture.h"
#include "Enums.h"
#include "D:/Dokumente/Unreal Engine/MC_Fake/Plugins/UnrealFastNoise-master/Source/UnrealFastNoisePlugin/Public/FastNoise/FastNoise.h"


ChunkGenerator::~ChunkGenerator()
{
	if (bUsesThread)
		delete ThisThread;
}

uint32 ChunkGenerator::Run()
{
	while (bRun)
	{
		while (bIsDone)
		{ 
			//wait 
		}
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
	PerlinNoise p(54658);
	UFastNoise* HeightNoise = NewObject<UFastNoise>();
	HeightNoise->SetNoiseType(ENoiseType::SimplexFractal);
	HeightNoise->SetFractalOctaves(6);
	HeightNoise->SetFrequency(1.f / 600.f);
	UFastNoise * TurbulenceNoise = NewObject<UFastNoise>();
	TurbulenceNoise->SetFrequency(1.f / 40.f);
	TurbulenceNoise->SetNoiseType(ENoiseType::SimplexFractal);
	TurbulenceNoise->SetFractalOctaves(2);
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
			for (int z = 0; z < 256; z++)
			{
				//TODO generate heightmap
				int HeightMapX = x + PosX + TurbulenceNoise->GetNoise(x + PosX, z) * 13.f ;
				int HeightMapY = y + PosY + TurbulenceNoise->GetNoise(z, y + PosY) * 13.f;
				//float noise1 = p.OctaveNoise(HeightMapX, HeightMapY, 0, 50, 18, 3) + 30;
				float noise2 = HeightNoise->GetNoise(HeightMapX, HeightMapY) * 40.f + 15;
				if (z <= noise2)
					(*ChunkBlockData)[x][y][z] = World->GetBlock(TEnumAsByte<EAllBlocks>(BStone));
				/*else if (z < 13)
					(*ChunkBlockData)[x][y][z] = World->GetBlock(TEnumAsByte<EAllBlocks>(Water));*/
				else
					(*ChunkBlockData)[x][y][z] = World->GetBlock(TEnumAsByte<EAllBlocks>(BAir));
			}
		}
	}
}

void ChunkGenerator::Stage1()
{
}

void ChunkGenerator::Stage2()
{
	for (int x = 0; x < 16; x++)
	{
		for (int y = 0; y < 16; y++)
		{
			for (int z = 0; z < 251; z++)
			{

				if (z < 250 && (*ChunkBlockData)[x][y][z] && (*ChunkBlockData)[x][y][z + 5] &&
					(*ChunkBlockData)[x][y][z]->GetBlockEnum() == BStone && (*ChunkBlockData)[x][y][z + 5]->GetBlockEnum() == BAir)
					(*ChunkBlockData)[x][y][z] = World->GetBlock(BDirt);
				if (z < 254 && (*ChunkBlockData)[x][y][z] && (*ChunkBlockData)[x][y][z + 1] &&
					(*ChunkBlockData)[x][y][z]->GetBlockEnum() == BDirt && (*ChunkBlockData)[x][y][z + 1]->GetBlockEnum() == BAir)
				{
					(*ChunkBlockData)[x][y][z] = World->GetBlock(BGrass);

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