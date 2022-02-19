#include "McWorld.h"
#include "../Blocks/B_Air.h"
#include "../Blocks/B_Stone.h"
#include "../Blocks/B_Dirt.h"
#include "../Blocks/B_Grass.h"
#include "../Blocks/B_Water.h"
#include "../Blocks/B_LogOak.h"
#include "../Blocks/B_LeavesOak.h"
#include "Engine/World.h"
#include "Chunk.h"
#include "ChunkCube.h"
#include "Matrix3x4.h"
#include "Core/Public/HAL/RunnableThread.h"
#include "Kismet/GameplayStatics.h"
#include "../Misc/FileIO.h"
#include "Blocks/BlockManager.h"
#include "Misc/DateTime.h"



AMcWorld::AMcWorld()
{
	PrimaryActorTick.bCanEverTick = true;

	ChunksRoot = CreateDefaultSubobject <USceneComponent>(TEXT("Chunks Root"));
	SetRootComponent(ChunksRoot);
	
	static bool bHaveClonablesBeenInitialized = false;
	if (!bHaveClonablesBeenInitialized)
	{
		BlockManager::InitializeAll();
		bHaveClonablesBeenInitialized = true;
	}

}

void AMcWorld::BeginPlay()
{
	Super::BeginPlay();

	const int numOfThreads = 4;
	GeneratorThreads.SetNum(numOfThreads);
	for (int i = 0; i < numOfThreads; ++i)
	{
		GeneratorThreads[i] = new ChunkGenerator;
		GeneratorThreads[i]->SetWorld(this);
		FRunnableThread* T = FRunnableThread::Create(GeneratorThreads[i], TEXT("Chunk Generator Thread"), 0, TPri_Lowest);
		GeneratorThreads[i]->ThisThread = T;
	}

	MeshGeneratorThreads.SetNum(4);
	for (auto& t : MeshGeneratorThreads)
	{
		t = new ChunkMeshGeneratorThread;
		t->SetWorld(this);
	}
}

void AMcWorld::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	DequeueChunkCubeGenTasks();
	DequeueChunkGenTasks();
	DequeueMeshGenTasks();

	++currTick;
}

void AMcWorld::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	bHasBeenDestroyed = true;
	Super::EndPlay(EndPlayReason);

	UE_LOG(LogTemp, Warning, TEXT("Destroying McWorld!"));
	for (int i = 0; i < GeneratorThreads.Num(); ++i)
	{
		GeneratorThreads[i]->bRun = false;
		FPlatformProcess::Sleep(0.005);
		GeneratorThreads[i]->ThisThread->Kill(true);
		delete GeneratorThreads[i];
	}
	GeneratorThreads.Empty();

	for (int i = 0; i < MeshGeneratorThreads.Num(); ++i)
	{
		MeshGeneratorThreads[i]->bRun = false;
		FPlatformProcess::Sleep(0.01);
		MeshGeneratorThreads[i]->thisThread->Kill(false);
		delete MeshGeneratorThreads[i];
	}
	MeshGeneratorThreads.Empty();

	TArray<UChunk*> AllChunks;
	LoadedChunks.GenerateValueArray(AllChunks);
	for (auto chunk : AllChunks)
		delete chunk;
	LoadedChunks.Empty();

	LoadedChunkCubes.Empty();
	

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
				--NumChunkGenBuffer;
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
			--NumChunkCubeGenBuffer;
		}
	}
}

void AMcWorld::DequeueMeshGenTasks()
{
	MeshGenBufferMutex.Lock();
	if (!MeshGenBuffer.IsEmpty())
	{
		bool foundReadyThread = false;
		for (auto thread : MeshGeneratorThreads)
		{
			if (!thread->bIsBusy && !MeshGenBuffer.IsEmpty()) {
				foundReadyThread = true;
				UChunkCube* newCube;
				MeshGenBuffer.Dequeue(newCube);
				thread->SetChunkCube(newCube);
				--NumMeshGenBuffer;
			}
		}

	}
	MeshGenBufferMutex.Unlock();
}

void AMcWorld::CompleteBlockSetTasks(UChunkCube * ChunkCube)
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
	//if (bDataChanged)
		//ChunkCube->SetHasBlockDataChanged();

}

void AMcWorld::FinalizeChunkGen(UChunk* Chunk)
{
	FChunkFormCoords2D p = Chunk->GetPos();

	Chunk->SetHasFinishedGenerating(true);
	if (ChunkCubeGenTasks.Contains(p))
	{
		ChunkCubesGenQueueMutex.Lock();
		ChunkCubeGenBuffer.Enqueue(ChunkCubeGenTasks.FindAndRemoveChecked(p));
		ChunkCubesGenQueueMutex.Unlock();
		++NumChunkCubeGenBuffer;
	}
}

void AMcWorld::FinalizeCubeGen(UChunkCube* FinishedChunkCube, FChunkFormCoords3D CurrChunkPos)
{
	NeighborUpdatesMutex.Lock();

	CompleteBlockSetTasks(FinishedChunkCube);

	FinishedChunkCube->SetNextGenerationStage(255);

	
	FChunkFormCoords3D key = CurrChunkPos;
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

UChunk* AMcWorld::GetChunkAt(FChunkFormCoords2D Location)
{
	UChunk** FoundChunk = LoadedChunks.Find(Location);
	return FoundChunk ? *FoundChunk : nullptr;
}

UChunk* AMcWorld::CreateChunk(FChunkFormCoords2D Location)
{
	if (UChunk* TmpChunk = GetChunkAt(Location))
		return TmpChunk;
	// UChunk* NewChunk = new UChunk(Location, this);
	UChunk* NewChunk = NewObject<UChunk>(this);
	NewChunk->RegisterComponent();
	NewChunk->Init(Location, this);
	NewChunk->AttachToComponent(ChunksRoot, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	ChunkGenBuffer.Enqueue({ Location.X, Location.Y, NewChunk }); 
	
	LoadedChunks.Add(Location, NewChunk);

	return NewChunk;
}

UChunkCube* AMcWorld::GetChunkCubeAt(const FChunkFormCoords3D& Location)
{
	UChunkCube** FoundChunkC = LoadedChunkCubes.Find(Location);
	return FoundChunkC ? *FoundChunkC : nullptr;
}

void AMcWorld::AddChunkGenTask(UChunkCube* Cube)
{

	UChunk** c = LoadedChunks.Find(Cube->GetPos().To2D());
	if (c && (*c)->GetHasFinishedGenerating())
	{
		ChunkCubeGenBuffer.Enqueue({ { Cube, 0 } });
		++NumChunkCubeGenBuffer;
	}
	else
	{
		FChunkFormCoords2D key2D = Cube->GetPos().To2D();

		if (!ChunkCubeGenTasks.Contains(key2D))
			ChunkCubeGenTasks.Add(key2D, TArray<ChunkCubeGenBufferElement>());

		ChunkCubeGenTasks[key2D].Add({ Cube, 0 });
	}
}

void AMcWorld::AddMeshGenTask(UChunkCube* Cube)
{
	MeshGenBufferMutex.Lock();
	MeshGenBuffer.Enqueue(Cube);
	MeshGenBufferMutex.Unlock();
	++NumMeshGenBuffer;
}

void AMcWorld::AddLoadedChunkCube(UChunkCube* Cube, FChunkFormCoords3D CurrChunkPos)
{
	// UE_LOG(LogTemp, Warning, TEXT("Adding CC at %s"), *CurrChunkPos.toString());
	LoadedChunkCubes.Add(CurrChunkPos, Cube);
}

void AMcWorld::RemoveLoadedChunkCube(FChunkFormCoords3D CurrChunkPos)
{
	if (bHasBeenDestroyed)		// ChunkCube::EndPlay can get called multiple times
		return;
	// UE_LOG(LogTemp, Warning, TEXT("Removing CC at %s"), *CurrChunkPos.toString());	
	LoadedChunkCubes.FindAndRemoveChecked(CurrChunkPos);
	ChunkCubeGenTasks.Remove(CurrChunkPos.To2D());
}

void AMcWorld::AddRemovedChunk(UChunk* chunk)
{
	RemovedChunksInGenBuffer.Add(chunk);
}

void AMcWorld::RemoveLoadedChunk(FChunkFormCoords2D Pos)
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

void AMcWorld::AddBlockSetTask(int32 x, int32 y, int32 z, class B_Block* NewBlock, uint8 MinGenStage, bool bUpdateMesh)
{
	BlockSetBufferElement e(x, y, z, NewBlock, MinGenStage);
	if (LoadedChunkCubes.Contains(e.CurrChunkPos) && LoadedChunkCubes[e.CurrChunkPos]->GetNextGenerationStage() >= MinGenStage)
	{
		auto& BlockData = LoadedChunkCubes[e.CurrChunkPos]->GetBlockData();
		BlockData[e.RelX][e.RelY][e.RelZ] = NewBlock;
		if (bUpdateMesh)
			LoadedChunkCubes[e.CurrChunkPos]->SetHasBlockDataChanged(true);
	}
	else
	{
		if (!BlockSetTasks.Contains(e.CurrChunkPos))
			BlockSetTasks.Add(e.CurrChunkPos, TArray<BlockSetBufferElement>());
		BlockSetTasks[e.CurrChunkPos].Add(e);
	}
}

