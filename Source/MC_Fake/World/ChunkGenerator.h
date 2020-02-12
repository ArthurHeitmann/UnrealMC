

#pragma once

#include "CoreMinimal.h"
#include "Core/Public/HAL/Runnable.h"
#include "Queue.h"
#include "McWorld.h"
#include "McWorldStructs.h"
#include "ChunkFormCoords.h"
#include "Enums.h"

/**
 * 
 */
class MC_FAKE_API ChunkGenerator : public FRunnable
{
private:
	const float SLOPE_GRASS = 5;
	const float SLOPE_DIRT = 7;

	bool bGenerateChunk = false;
	bool bGenerateChunkCube = false;
	ChunkFormCoords3D Pos;
	ChunkFormCoords2D Pos2D;
	TArray<ChunkCubeGenBufferElement> CubeGenerationList;
	ChunkCubeGenBufferElement CurrentCubeElement;
	class Chunk* CurrentChunk;
	class AMcWorld* World;
	int NextGenerationStage;
	int LastGenerationStage;

	class B_Block* GetBlockAt(int32 X, int32 Y, int32 Z);
	void SetBlockAt(int32 X, int32 Y, int32 Z, class B_Block* Block);

	void GenerateChunkData();
	void GenerateChunkCubes();

	void GenHeightMap();

	void Stage_BaseStoneTerrain();
	void Stage_DirtGrass();
	void Stage_CaveCarving();
	void Stage_Trees();

	bool HasAirInRange(FName* NextBlocks, int Start, int End);

	void Wait();

public:
	bool bRun = true;
	bool bIsBusy;
	FRunnableThread* ThisThread;

	~ChunkGenerator();
	virtual uint32 Run() override;
	void SetChunkData(class Chunk* WorkingChunk);
	void SetCubesData(TArray<ChunkCubeGenBufferElement>& Cubes);
	void SetWorld(class AMcWorld* NewWorld);

};
