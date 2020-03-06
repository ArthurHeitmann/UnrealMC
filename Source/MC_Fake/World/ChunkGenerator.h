

#pragma once

#include "CoreMinimal.h"
#include "Core/Public/HAL/Runnable.h"
#include "ChunkMeshGenerator.h"
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

	class UFastNoise* HeightNoise;
	UFastNoise* TurbulenceNoise;
	UFastNoise* CaveNoise1;
	
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
	static bool bEnableStageDirtGrass;
	static bool bEnableStageCaveCarving;
	static bool bEnableStageTrees;
	static float BaseTerrainHeight;
	static float TerrainHeightMultiplier;
	static float TerrainTurbulenceMultiplier;
	static int HeightMapOctaves;
	static float HeightMapFrequency;
	static int TurbulenceOctaves;
	static float TurbulenceFrequency;
	static float SlopeGrass;
	static float SlopeDirt;
	static int CaveOctaves;
	static float CaveFrequency;
	static float CaveThreshold;
	
	bool bRun = true;
	bool bIsBusy;
	FRunnableThread* ThisThread;

	~ChunkGenerator();
	ChunkGenerator();
	virtual uint32 Run() override;
	void SetChunkData(class Chunk* WorkingChunk);
	void SetCubesData(TArray<ChunkCubeGenBufferElement>& Cubes);
	void SetWorld(class AMcWorld* NewWorld);

};
