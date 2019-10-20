

#pragma once

#include "CoreMinimal.h"
#include "Core/Public/HAL/Runnable.h"
#include "Queue.h"

/**
 * 
 */
class MC_FAKE_API ChunkGenerator : public FRunnable
{
private:
	//bool bIsDone = true;
	bool bGenerateChunk = false;
	bool bGenerateChunkCube = false;
	int PosX, PosY;
	TQueue<class UChunkCube*> CubeGenerationQueue;
	class UChunkCube* CurrentCube;
	//TArray<TArray<TArray<class Block*>>>* ChunkBlockData;
	class AChunk* Chunk;
	class AMcWorld* World;
	int NextGenerationStage;
	int LastGenerationStage;


	void GenerateChunkData();
	void GenerateChunkCubes();

	void GenHeightMap();

	void Stage_BaseStoneTerrain();
	void Stage_DirtGrass();
	void Stage_CaveCarving();
	void Stage_Trees();

	void wait();

public:
	bool bRun = true;
	//bool bUsesThread;
	FRunnableThread* ThisThread;

	~ChunkGenerator();
	virtual uint32 Run();
	//void Reset(int x, int y, class AChunk* WorkingChunk, int NewGenerationStage, int MaxGenStage = 255, bool bUseThread = true);
	void SetChunkData(int x, int y, class AChunk* WorkingChunk);
	void SetCubesData(const TArray<class UChunkCube*>& Cubes, int NewGenerationStage, int MaxGenStage = 255);
	void SetWorld(class AMcWorld* NewWorld);
	//void Generate();

	void GenerateChunkCube();
};
