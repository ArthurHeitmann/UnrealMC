#pragma once

#include "CoreMinimal.h"
#include "Enums.h"
#include "Core/Public/HAL/Runnable.h"
#include "Core/Public/HAL/RunnableThread.h"

class ChunkMeshGenerator
{
protected:
	static bool ShouldFaceBePlacedBetween(class B_Block* b1, B_Block* b2, EDirection Side);
public:
	static void GenerateChunkMesh(class UChunkCube* C);
};

class ChunkMeshGeneratorThread : public FRunnable
{
	UChunkCube* Cube = nullptr;
	bool bNewTask = false;
	class AMcWorld* World;
public:
	FRunnableThread* thisThread = nullptr;
	bool bRun = true;
	bool bIsBusy = false;

	ChunkMeshGeneratorThread();
	void SetChunkCube(class UChunkCube* C);
	void SetWorld(AMcWorld* W);
	virtual uint32 Run() override;
};