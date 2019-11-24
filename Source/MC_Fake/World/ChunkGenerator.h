

#pragma once

#include "CoreMinimal.h"
#include "Core/Public/HAL/Runnable.h"
#include "Queue.h"
#include "McWorld.h"
#include "McWorldStructs.h"
#include "ChunkFormCoords.h"

/**
 * 
 */
class MC_FAKE_API ChunkGenerator : public FRunnable
{
private:
	//bool bIsDone = true;
	bool bGenerateChunk = false;
	bool bGenerateChunkCube = false;
	//int PosX, PosY;		TODO CR
	ChunkFormCoords3D Pos;
	ChunkFormCoords2D Pos2D;
	TArray<ChunkCubeGenBufferElement> CubeGenerationList;
	ChunkCubeGenBufferElement CurrentCubeElement;
	//TArray<TArray<TArray<class Block*>>>* ChunkBlockData;
	class Chunk* CurrentChunk;
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
	bool bIsBusy;
	FRunnableThread* ThisThread;

	~ChunkGenerator();
	virtual uint32 Run();
	//void Reset(int x, int y, class Chunk* WorkingChunk, int NewGenerationStage, int MaxGenStage = 255, bool bUseThread = true);
	void SetChunkData(class Chunk* WorkingChunk);
	void SetCubesData(TArray<ChunkCubeGenBufferElement>& Cubes);
	void SetWorld(class AMcWorld* NewWorld);
	//void Generate();

	void GenerateChunkCube();
};
