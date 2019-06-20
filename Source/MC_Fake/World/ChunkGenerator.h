

#pragma once

#include "CoreMinimal.h"
#include "Core/Public/HAL/Runnable.h"

/**
 * 
 */
class MC_FAKE_API ChunkGenerator : public FRunnable
{
private:
	int PosX, PosY;
	TArray<TArray<TArray<class Block*>>>* ChunkBlockData;
	class AChunk* Chunk;
	class AMcWorld* World;
	int NextGenerationStage;
	int LastGenerationStage;
	void Stage0();
	void Stage1();
	void Stage2();
	void Stage3();
	void Stage4();
	void Stage5();
	void Stage6();

public:
	~ChunkGenerator();
	virtual uint32 Run();
	void Reset(int x, int y, class AChunk* WorkingChunk, int NewGenerationStage, int MaxGenStage = 255, bool bUseThread = true);
	void SetWorld(class AMcWorld* NewWorld);
	void Generate();

	bool bIsDone = true;
	bool bRun = true;
	bool bUsesThread;
	FRunnableThread* ThisThread;
};
