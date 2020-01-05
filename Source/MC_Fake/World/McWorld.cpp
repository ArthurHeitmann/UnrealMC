#include "McWorld.h"
#include "../Blocks/B_Air.h"
#include "../Blocks/B_Stone.h"
#include "../Blocks/B_Dirt.h"
#include "../Blocks/B_Grass.h"
#include "../Blocks/B_Water.h"
#include "../Blocks/B_LogOak.h"
#include "../Blocks/B_Leaves.h"
#include "Engine/World.h"
#include "Chunk.h"
#include "ChunkCube.h"
#include "Core/Public/HAL/RunnableThread.h"
#include "Kismet/GameplayStatics.h"
#include "../Misc/FileIO.h"

AMcWorld::AMcWorld()
{
	PrimaryActorTick.bCanEverTick = true;

	ChunksRoot = CreateDefaultSubobject <USceneComponent>(TEXT("Chunks Root"));

	if (!B_Air::AirRef)
		B_Air::AirRef = (new B_Air());
	B_Stone();
	B_Dirt();
	B_Grass();
	B_Water();
	B_Leaves();
	B_LogOak();
}

void AMcWorld::BeginPlay()
{
	Super::BeginPlay();

	const int numOfThreads = 3;
	GeneratorThreads.SetNum(numOfThreads);
	for (int i = 0; i < numOfThreads; ++i)
	{
		GeneratorThreads[i] = new ChunkGenerator;
		GeneratorThreads[i]->SetWorld(this);
		FRunnableThread* T = FRunnableThread::Create(GeneratorThreads[i], TEXT("Chunk Generator Thread"), 0, TPri_Lowest);
		GeneratorThreads[i]->ThisThread = T;
	}
}

void AMcWorld::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (auto& chunk : LoadedChunks)
		chunk.Value->Tick(DeltaTime);

	DequeueChunkCubeGenTasks();
	DequeueChunkGenTasks();
}

void AMcWorld::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	UE_LOG(LogTemp, Warning, TEXT("Destroying McWorld!"));
	for (int i = 0; i < GeneratorThreads.Num(); ++i)
	{
		GeneratorThreads[i]->bRun = false;
		GeneratorThreads[i]->ThisThread->Kill(true);
		delete GeneratorThreads[i];
	}
	GeneratorThreads.Empty();
}

void AMcWorld::DequeueChunkGenTasks()
{
	for (ChunkGenerator* Thread : GeneratorThreads)
	{
		if (ChunkGenBuffer.IsEmpty())
			break;
		if (!Thread->bIsBusy)
		{
			ChunkGenBufferElement Element;
			ChunkGenBuffer.Dequeue(Element);
			while (RemovedChunksInGenBuffer.Contains(Element.Chunk)) {
				RemovedChunksInGenBuffer.RemoveSwap(Element.Chunk);
				ChunkGenBuffer.Dequeue(Element);
			}
			Thread->SetChunkData(Element.Chunk);
		}
	}
}

void AMcWorld::DequeueChunkCubeGenTasks()
{
	for (ChunkGenerator* Thread : GeneratorThreads)
	{
		if (ChunkCubeGenBuffer.IsEmpty())
			break;
		if (!Thread->bIsBusy)
		{
			TArray<ChunkCubeGenBufferElement> ChunkCubesBElement;
			ChunkCubeGenBuffer.Dequeue(ChunkCubesBElement);
			Thread->SetCubesData(ChunkCubesBElement);
		}
	}
}

void AMcWorld::CompleteBlockSetTasks(ChunkCube * ChunkCube)
{
	if (!BlockSetTasks.Contains(ChunkCube->GetPos()))
		return;

	auto& BlockData = ChunkCube->GetBlockData();
	int ChunkNextGenStage = ChunkCube->GetNextGenerationStage();
	bool bDataChanged = false;
	auto& Tasks = BlockSetTasks[ChunkCube->GetPos()];
	while (Tasks.Num())
	{
		if (Tasks[0].MinGenStage <= ChunkNextGenStage)
		{
			BlockData[Tasks[0].RelX][Tasks[0].RelY][Tasks[0].RelZ] = Tasks[0].BlockS;
			bDataChanged = true;
			Tasks.RemoveAtSwap(0);
		}
	}
	if (bDataChanged)
		ChunkCube->SetHasDataChanged();

}

void AMcWorld::FinalizeChunkGen(Chunk* Chunk)
{
	ChunkFormCoords2D p = Chunk->GetPos();

	Chunk->SetHasFinishedGenerating(true);
	if (ChunkCubeGenTasks.Contains(p))
	{
		ChunkCubesGenQueueMutex.Lock();
		ChunkCubeGenBuffer.Enqueue(ChunkCubeGenTasks.FindAndRemoveChecked(p));
		ChunkCubesGenQueueMutex.Unlock();
	}
}

void AMcWorld::FinalizeCubeGen(ChunkCube* FinishedChunkCube, ChunkFormCoords3D CurrChunkPos)
{
	NeighborUpdatesMutex.Lock();

	CompleteBlockSetTasks(FinishedChunkCube);

	FinishedChunkCube->SetNextGenerationStage(255);
	FinishedChunkCube->SetHasDataChanged();

	//TODO CR Check if all Trues for bUpdateMesh are necessary
	/*
	if (NeighborUpdateTasks.Contains(FinishedChunkCube))
	{
		ChunkNeighborUpdates& updates = NeighborUpdateTasks[FinishedChunkCube];
		if (updates.NewNChunk)
		{
			FinishedChunkCube->UpdateCubeNeighbor(NORTH, updates.NewNChunk, false);
			updates.NewNChunk->UpdateCubeNeighbor(SOUTH, FinishedChunkCube, true);
			updates.NewNChunk = nullptr;
		}
		if (updates.NewEChunk)
		{
			FinishedChunkCube->UpdateCubeNeighbor(EAST, updates.NewEChunk, false);
			updates.NewEChunk->UpdateCubeNeighbor(WEST, FinishedChunkCube, true);
			updates.NewEChunk = nullptr;
		}
		if (updates.NewSChunk)
		{
			FinishedChunkCube->UpdateCubeNeighbor(SOUTH, updates.NewSChunk, false);
			updates.NewSChunk->UpdateCubeNeighbor(NORTH, FinishedChunkCube, true);
			updates.NewSChunk = nullptr;
		}
		if (updates.NewWChunk)
		{
			FinishedChunkCube->UpdateCubeNeighbor(WEST, updates.NewWChunk, false);
			updates.NewWChunk->UpdateCubeNeighbor(EAST, FinishedChunkCube, true);
			updates.NewWChunk = nullptr;
		}
		if (updates.NewTChunk)
		{
			FinishedChunkCube->UpdateCubeNeighbor(TOP, updates.NewTChunk, false);
			updates.NewTChunk->UpdateCubeNeighbor(BOTTOM, FinishedChunkCube, true);
			updates.NewTChunk = nullptr;
		}
		if (updates.NewBChunk)
		{
			FinishedChunkCube->UpdateCubeNeighbor(BOTTOM, updates.NewBChunk, false);
			updates.NewBChunk->UpdateCubeNeighbor(TOP, FinishedChunkCube, true);
			updates.NewBChunk = nullptr;
		}

		NeighborUpdateTasks.Remove(FinishedChunkCube);
	}*/
	ChunkFormCoords3D key = CurrChunkPos;
	key.X--;
	if (LoadedChunkCubes.Contains(key)) 
	{
		if (LoadedChunkCubes[key]->GetHasFinishedGenerating())
		{
			LoadedChunkCubes[key]->UpdateCubeNeighbor(NORTH, FinishedChunkCube, true);
			FinishedChunkCube->UpdateCubeNeighbor(SOUTH, LoadedChunkCubes[key], true);
		}
		else
			NeighborUpdateTasks.Add(LoadedChunkCubes[key], ChunkNeighborUpdates{ FinishedChunkCube, 0, 0, 0, 0, 0 });
	}
	key.X += 2;
	if (LoadedChunkCubes.Contains(key))
	{
		if (LoadedChunkCubes[key]->GetHasFinishedGenerating())
		{
			LoadedChunkCubes[key]->UpdateCubeNeighbor(SOUTH, FinishedChunkCube, true);
			FinishedChunkCube->UpdateCubeNeighbor(NORTH, LoadedChunkCubes[key], true);
		}
		else
			NeighborUpdateTasks.Add(LoadedChunkCubes[key], ChunkNeighborUpdates{ 0, 0, FinishedChunkCube, 0, 0, 0 });
	}
	key.X--;
	key.Y--;
	if (LoadedChunkCubes.Contains(key))
	{
		if (LoadedChunkCubes[key]->GetHasFinishedGenerating())
		{
			LoadedChunkCubes[key]->UpdateCubeNeighbor(EAST, FinishedChunkCube, true);
			FinishedChunkCube->UpdateCubeNeighbor(WEST, LoadedChunkCubes[key], true);
		}
		else
			NeighborUpdateTasks.Add(LoadedChunkCubes[key], ChunkNeighborUpdates{ 0, FinishedChunkCube, 0, 0, 0, 0 });
	}
	key.Y += 2;
	if (LoadedChunkCubes.Contains(key))
	{
		if (LoadedChunkCubes[key]->GetHasFinishedGenerating())
		{
			LoadedChunkCubes[key]->UpdateCubeNeighbor(WEST, FinishedChunkCube, true);
			FinishedChunkCube->UpdateCubeNeighbor(EAST, LoadedChunkCubes[key], true);
		}
		else
			NeighborUpdateTasks.Add(LoadedChunkCubes[key], ChunkNeighborUpdates{ 0, 0, 0, FinishedChunkCube, 0, 0 });
	}
	key.Y--;
	key.Z--;
	if (LoadedChunkCubes.Contains(key))
	{
		if (LoadedChunkCubes[key]->GetHasFinishedGenerating())
		{
			LoadedChunkCubes[key]->UpdateCubeNeighbor(TOP, FinishedChunkCube, true);
			FinishedChunkCube->UpdateCubeNeighbor(BOTTOM, LoadedChunkCubes[key], true);
		}
		else
			NeighborUpdateTasks.Add(LoadedChunkCubes[key], ChunkNeighborUpdates{ 0, 0, 0, 0, FinishedChunkCube, 0 });
	}
	key.Z += 2;
	if (LoadedChunkCubes.Contains(key))
	{
		if (LoadedChunkCubes[key]->GetHasFinishedGenerating())
		{
			LoadedChunkCubes[key]->UpdateCubeNeighbor(BOTTOM, FinishedChunkCube, true);
			FinishedChunkCube->UpdateCubeNeighbor(TOP, LoadedChunkCubes[key], true);
		}
		else
			NeighborUpdateTasks.Add(LoadedChunkCubes[key], ChunkNeighborUpdates{ 0, 0, 0, 0, FinishedChunkCube });
	}


	FinishedChunkCube->SetHasFinishedGenerating();

	NeighborUpdatesMutex.Unlock();
}

B_Block* AMcWorld::GetBlockFromEnum(EAllBlocks Block)
{
	switch (Block)
	{
	case BAir:
		return B_Air::AirRef;
	case BStone:
		return new B_Stone();
	case BDirt:
		return new B_Dirt();
	case BGrass:
		return new B_Grass();
	case BWater:
		return new B_Water();
	case BLeaves_Oak:
		return new B_Leaves();
	case BLog_Oak:
		return new B_LogOak();
	}

	return B_Air::AirRef;
}

Chunk* AMcWorld::GetChunkAt(ChunkFormCoords2D Location)
{
	Chunk** FoundChunk = LoadedChunks.Find(Location);
	return FoundChunk ? *FoundChunk : nullptr;
}

Chunk* AMcWorld::CreateChunk(ChunkFormCoords2D Location)
{
	if (Chunk* TmpChunk = GetChunkAt(Location))
		return TmpChunk;
	Chunk* NewChunk = new Chunk(Location, this);
	ChunkGenBuffer.Enqueue({ Location.X, Location.Y, NewChunk }); 
	
	LoadedChunks.Add(Location, NewChunk);

	
	return NewChunk;
}

ChunkCube* AMcWorld::GetChunkCubeAt(const ChunkFormCoords3D& Location)
{
	ChunkCube** FoundChunkC = LoadedChunkCubes.Find(Location);
	return FoundChunkC ? *FoundChunkC : nullptr;
}

void AMcWorld::AddChunkGenTask(ChunkCube* Cube)
{

	Chunk** c = LoadedChunks.Find(Cube->GetPos().To2D());
	if (c && (*c)->GetHasFinishedGenerating())
		ChunkCubeGenBuffer.Enqueue({ { Cube, 0 } });
	else
	{
		ChunkFormCoords2D key2D = Cube->GetPos().To2D();

		if (!ChunkCubeGenTasks.Contains(key2D))
			ChunkCubeGenTasks.Add(key2D, TArray<ChunkCubeGenBufferElement>());

		ChunkCubeGenTasks[key2D].Add({ Cube, 0 });
	}
}

void AMcWorld::AddLoadedChunkCube(ChunkCube* Cube, ChunkFormCoords3D CurrChunkPos)
{
	LoadedChunkCubes.Add(CurrChunkPos, Cube);
}

void AMcWorld::RemoveLoadedChunkCube(ChunkFormCoords3D CurrChunkPos)
{
	LoadedChunkCubes.FindAndRemoveChecked(CurrChunkPos);
	ChunkCubeGenTasks.Remove(CurrChunkPos.To2D());
}

void AMcWorld::AddRemovedChunk(Chunk* chunk)
{
	RemovedChunksInGenBuffer.Add(chunk);
}

void AMcWorld::RemoveLoadedChunk(ChunkFormCoords2D Pos)
{
	LoadedChunks.Remove(Pos);
}

void AMcWorld::QuickSave()
{
}

void AMcWorld::QuickLoad()
{
}

B_Block* AMcWorld::GetBlockAt(int32 x, int32 y, int32 z, bool bLoadChunkIfNeeded = false, int MinGenStage, int maxGenStage)
{
	int32 ChunkX = x / 16;
	int32 ChunkY = y / 16;
	int32 ChunkZ = z / 16;
	int32 relX = x % 16;
	int32 relY = y % 16;
	int32 relZ = z % 16;
	if (relX < 0)
		relX += 16;
	if (relY < 0)
		relY += 16;
	if (LoadedChunkCubes.Contains({ ChunkX, ChunkY, ChunkZ }))
	{
		auto& data = LoadedChunkCubes[{ ChunkX, ChunkY, ChunkZ }]->GetBlockData();
		return data[relX][relY][z];
	}

	return nullptr;
}

void AMcWorld::AddBlockSetTask(int32 x, int32 y, int32 z, class B_Block* NewBlock, uint8 MinGenStage)
{
	BlockSetBufferElement e(x, y, z, NewBlock, MinGenStage);
	if (LoadedChunkCubes.Contains(e.CurrChunkPos) && LoadedChunkCubes[e.CurrChunkPos]->GetNextGenerationStage() >= MinGenStage)
	{
		auto& BlockData = LoadedChunkCubes[e.CurrChunkPos]->GetBlockData();
		BlockData[e.RelX][e.RelY][e.RelZ] = NewBlock;
		LoadedChunkCubes[e.CurrChunkPos]->SetHasDataChanged(true);
	}
	else
	{
		if (!BlockSetTasks.Contains(e.CurrChunkPos))
			BlockSetTasks.Add(e.CurrChunkPos, TArray<BlockSetBufferElement>());
		BlockSetTasks[e.CurrChunkPos].Add(e);
	}
}

