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
#include "ChunkSaveGame.h"
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
	//GeneratorThreads.SetNum(FPlatformMisc::NumberOfCoresIncludingHyperthreads() / 2);
	//for (int i = 0; i < FPlatformMisc::NumberOfCoresIncludingHyperthreads() / 2; i++)
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
	{
		chunk.Value->Tick(DeltaTime);
	}

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
		//GeneratorThreads[i]->ThisThread->Suspend(true);
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
			ChunkFormCoords2D key2D = { Element.x, Element.y };//TODO CR what is this
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

void AMcWorld::CompleteBlockSetTasks(ChunkCube * ChunkCube, int32 ChunkX, int32 ChunkY, int32 ChunkZ)
{
	ChunkFormCoords3D key = { ChunkX, ChunkY, ChunkZ };
	if (!BlockSetTasks.Contains(key))
		return;

	auto& BlockData = ChunkCube->GetBlockData();
	int ChunkNextGenStage = ChunkCube->GetNextGenerationStage();
	bool bDataChanged = false;
	auto& Tasks = BlockSetTasks[key];
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
		ChunkCubeGenBuffer.Enqueue(ChunkCubeGenTasks.FindAndRemoveChecked(p));
}

void AMcWorld::FinalizeCubeGen(ChunkCube* FinishedChunkCube, ChunkFormCoords3D CurrChunkPos)
{
	NeighbourUpdatesMutex.Lock();

	FinishedChunkCube->SetNextGenerationStage(255);
	FinishedChunkCube->SetHasDataChanged();

	//TODO CR Check if all Trues for bUpdateMesh are necessary

	if (NeighbourUpdateTasks.Contains(FinishedChunkCube))
	{
		ChunkNeighbourUpdates& updates = NeighbourUpdateTasks[FinishedChunkCube];
		if (updates.NewNChunk)
		{
			FinishedChunkCube->UpdateCubeNeighbour(NORTH, updates.NewNChunk, false);
			updates.NewNChunk->UpdateCubeNeighbour(SOUTH, FinishedChunkCube, true);
			updates.NewNChunk = nullptr;
		}
		if (updates.NewEChunk)
		{
			FinishedChunkCube->UpdateCubeNeighbour(EAST, updates.NewEChunk, false);
			updates.NewEChunk->UpdateCubeNeighbour(WEST, FinishedChunkCube, true);
			updates.NewEChunk = nullptr;
		}
		if (updates.NewSChunk)
		{
			FinishedChunkCube->UpdateCubeNeighbour(SOUTH, updates.NewSChunk, false);
			updates.NewSChunk->UpdateCubeNeighbour(NORTH, FinishedChunkCube, true);
			updates.NewSChunk = nullptr;
		}
		if (updates.NewWChunk)
		{
			FinishedChunkCube->UpdateCubeNeighbour(WEST, updates.NewWChunk, false);
			updates.NewWChunk->UpdateCubeNeighbour(EAST, FinishedChunkCube, true);
			updates.NewWChunk = nullptr;
		}
		if (updates.NewTChunk)
		{
			FinishedChunkCube->UpdateCubeNeighbour(TOP, updates.NewTChunk, false);
			updates.NewTChunk->UpdateCubeNeighbour(BOTTOM, FinishedChunkCube, true);
			updates.NewTChunk = nullptr;
		}
		if (updates.NewBChunk)
		{
			FinishedChunkCube->UpdateCubeNeighbour(BOTTOM, updates.NewBChunk, false);
			updates.NewBChunk->UpdateCubeNeighbour(TOP, FinishedChunkCube, true);
			updates.NewBChunk = nullptr;
		}

		NeighbourUpdateTasks.Remove(FinishedChunkCube);
	}
	ChunkFormCoords3D key = CurrChunkPos;
	key.x--;
	if (LoadedChunkCubes.Contains(key)) 
	{
		if (LoadedChunkCubes[key]->GetHasFinishedGenerating())
		{
			LoadedChunkCubes[key]->UpdateCubeNeighbour(NORTH, FinishedChunkCube, true);
			FinishedChunkCube->UpdateCubeNeighbour(SOUTH, LoadedChunkCubes[key], true);
		}
		else
			NeighbourUpdateTasks.Add(LoadedChunkCubes[key], ChunkNeighbourUpdates{ FinishedChunkCube, 0, 0, 0, 0, 0 });
	}
	key.x += 2;
	if (LoadedChunkCubes.Contains(key))
	{
		if (LoadedChunkCubes[key]->GetHasFinishedGenerating())
		{
			LoadedChunkCubes[key]->UpdateCubeNeighbour(SOUTH, FinishedChunkCube, true);
			FinishedChunkCube->UpdateCubeNeighbour(NORTH, LoadedChunkCubes[key], true);
		}
		else
			NeighbourUpdateTasks.Add(LoadedChunkCubes[key], ChunkNeighbourUpdates{ 0, 0, FinishedChunkCube, 0, 0, 0 });
	}
	key.x--;
	key.y--;
	if (LoadedChunkCubes.Contains(key))
	{
		if (LoadedChunkCubes[key]->GetHasFinishedGenerating())
		{
			LoadedChunkCubes[key]->UpdateCubeNeighbour(EAST, FinishedChunkCube, true);
			FinishedChunkCube->UpdateCubeNeighbour(WEST, LoadedChunkCubes[key], true);
		}
		else
			NeighbourUpdateTasks.Add(LoadedChunkCubes[key], ChunkNeighbourUpdates{ 0, FinishedChunkCube, 0, 0, 0, 0 });
	}
	key.y += 2;
	if (LoadedChunkCubes.Contains(key))
	{
		if (LoadedChunkCubes[key]->GetHasFinishedGenerating())
		{
			LoadedChunkCubes[key]->UpdateCubeNeighbour(WEST, FinishedChunkCube, true);
			FinishedChunkCube->UpdateCubeNeighbour(EAST, LoadedChunkCubes[key], true);
		}
		else
			NeighbourUpdateTasks.Add(LoadedChunkCubes[key], ChunkNeighbourUpdates{ 0, 0, 0, FinishedChunkCube, 0, 0 });
	}
	key.y--;
	key.z--;
	if (LoadedChunkCubes.Contains(key))
	{
		if (LoadedChunkCubes[key]->GetHasFinishedGenerating())
		{
			LoadedChunkCubes[key]->UpdateCubeNeighbour(TOP, FinishedChunkCube, true);
			FinishedChunkCube->UpdateCubeNeighbour(BOTTOM, LoadedChunkCubes[key], true);
		}
		else
			NeighbourUpdateTasks.Add(LoadedChunkCubes[key], ChunkNeighbourUpdates{ 0, 0, 0, 0, FinishedChunkCube, 0 });
	}
	key.z += 2;
	if (LoadedChunkCubes.Contains(key))
	{
		if (LoadedChunkCubes[key]->GetHasFinishedGenerating())
		{
			LoadedChunkCubes[key]->UpdateCubeNeighbour(BOTTOM, FinishedChunkCube, true);
			FinishedChunkCube->UpdateCubeNeighbour(TOP, LoadedChunkCubes[key], true);
		}
		else
			NeighbourUpdateTasks.Add(LoadedChunkCubes[key], ChunkNeighbourUpdates{ 0, 0, 0, 0, FinishedChunkCube });
	}


	FinishedChunkCube->SetHasFinishedGenerating();

	NeighbourUpdatesMutex.Unlock();
}

Block* AMcWorld::GetBlockFromEnum(EAllBlocks Block)
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
	Chunk* NewChunk;
	//if (FileIO::DoesChunkExist("Debug_World", Location.X / 1600, Location.Y / 1600))
	//{
	//	ChunkData LoadedData(FileIO::LoadChunk("Debug_World", Location.X / 1600, Location.Y / 1600));
	//	//TODO check for same generator version
	//	Chunk = GetWorld()->SpawnActor<Chunk>({Location.X, Location.Y, 0.f}, FRotator::ZeroRotator);
	//	Chunk->SetData(LoadedData.BlockData, true);
	//	Chunk->SetNextGenerationStage(LoadedData.NextGenerationStage);
	//	Chunk->SetLastTimeUpdated(LoadedData.LastTimeUpdated);
	//	if (LoadedData.NextGenerationStage != 255)
	//		ChunkGenBuffer.Enqueue({ ( int) Location.X / 100, ( int) Location.Y / 100, Chunk, LoadedData.NextGenerationStage });
	//	else
	//	{
	//		int32 ChunkX = Location.X / 1600;
	//		int32 ChunkY = Location.Y / 1600;
	//		CompleteBlockSetTasks(Chunk, ChunkX, ChunkY, );
	//	}
	//}
	//else
	//{
	NewChunk = new Chunk(Location, this);
	//Chunk = GetWorld()->SpawnActor<Chunk>({Location.x * 1600.f, Location.y * 1600.f, 0.f}, FRotator::ZeroRotator);
	ChunkGenBuffer.Enqueue({ Location.x, Location.y, NewChunk }); 
	//}
	
	LoadedChunks.Add(Location, NewChunk);
	return NewChunk;
}

void AMcWorld::AddChunkGenTask(ChunkCube* Cube)
{
	ChunkFormCoords2D key2D = Cube->GetPos().To2D();
	if (!ChunkCubeGenTasks.Contains(key2D))
		ChunkCubeGenTasks.Add(key2D, TArray<ChunkCubeGenBufferElement>());
	ChunkCubeGenTasks[key2D].Add({ Cube, 0 });
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
	/*for (auto ChunkPair : LoadedChunks)
	{
		if (ChunkPair.Value)
			FileIO::SaveChunk(ChunkPair.Value, "Debug_World");
	}*/
}

void AMcWorld::QuickLoad()
{
	/*TArray<AActor*> AllChunks;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), Chunk::StaticClass(), AllChunks);
	for (AActor* ChunkA : AllChunks)
	{
		if (Chunk* Chunk = Cast<Chunk>(ChunkA))
		{
			int x = Chunk->GetActorLocation().X / 1600;
			int y = Chunk->GetActorLocation().Y / 1600;
			if (!FileIO::DoesChunkExist("Debug_World", x, y))
				continue;
			ChunkData CData(FileIO::LoadChunk("Debug_World", x, y));
			Chunk->SetData(CData.BlockData, true);
			ChunkGenBuffer.Enqueue({ x, y, Chunk, CData.NextGenerationStage }); 
			Chunk->SetNextGenerationStage(CData.NextGenerationStage);
		}
	}*/
}

//bool AMcWorld::SetBlockAt(int32 x, int32 y, int32 z, Block* Block, bool ForcePlacement, int MinGenStage, int maxGenStage)
//{
//	int32 ChunkX = x / 16;
//	int32 ChunkY = y / 16;
//	if (x < 0)
//		ChunkX--;
//	if (y < 0)
//		ChunkY--;
//	int32 relX = x % 16;
//	int32 relY = y % 16;
//	if (relX < 0)
//		relX += 16;
//	if (relY < 0)
//		relY += 16;
//	if (CurrentlyLoadedChunks.Contains({ ChunkX, ChunkY }) && CurrentlyLoadedChunks[{ ChunkX, ChunkY }]->GetNextGenerationStage() == 255)
//	{
//		Chunk* c = CurrentlyLoadedChunks[{ ChunkX, ChunkY }];
//		auto data = c->GetChunkBlockData();
//		(*data)[relX][relY][z] = Block;
//		c->SetHasDataChanged();
//		return true;
//	}
//	else if (ForcePlacement && FileIO::DoesChunkExist("Debug_World", ChunkX, ChunkY))
//	{
//		ChunkData data = FileIO::LoadChunk("Debug_World", ChunkX, ChunkY);		//TODO check GenVersion
//		Chunk* Chunk = GetWorld()->SpawnActor<Chunk>({ (float) (x - relX) * 100, (float) (y - relY) * 100, 0 }, FRotator::ZeroRotator);
//		CurrentlyLoadedChunks.Add({ ChunkX, ChunkY }, Chunk);
//		Chunk->SetData(data.BlockData, true);
//		Chunk->SetNextGenerationStage(data.NextGenerationStage);
//		Chunk->SetLastTimeUpdated(data.LastTimeUpdated);
//		if (data.NextGenerationStage != 255)
//		{
//			ChunkGenBuffer.Enqueue({ x - relX, y - relY, Chunk, data.NextGenerationStage });
//			ChunkGenBufferArray.Add({ x - relX, y - relY });
//			return false;
//		}
//		else 
//		{
//			auto BlockData = Chunk->GetChunkBlockData();
//			(*BlockData)[relX][relY][z] = Block;
//			return true;
//		}
//	}
//	else if (ForcePlacement && !DoesGenTaskAllreadyExists(x - relX, y - relY))
//	{
//		Chunk* Chunk = GetWorld()->SpawnActor<Chunk>({ (float) (x - relX) * 100, (float) (y - relY) * 100, 0 }, FRotator::ZeroRotator);
//		CurrentlyLoadedChunks.Add({ ChunkX, ChunkY}, Chunk);
//		ChunkGenBuffer.Enqueue({ x - relX, y - relY, Chunk, 0});
//		ChunkGenBufferArray.Add({ x - relX, y - relY });
//		return false;
//	}
//	return false;
//}

Block* AMcWorld::GetBlockAt(int32 x, int32 y, int32 z, bool bLoadChunkIfNeded = false, int MinGenStage, int maxGenStage)
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
	else if (bLoadChunkIfNeded && FileIO::DoesChunkExist("Debug_World", ChunkX, ChunkY))
	{
		ChunkData data = FileIO::LoadChunk("Debug_World", ChunkX, ChunkY);		//TODO check GenVersion
		return data.BlockData[relX][relY][relZ];
	}
	/*else if (ForceSuccess)
	{
		Chunk* Chunk = GetWorld()->SpawnActor<Chunk>({ (float) ChunkX, (float) ChunkY, (float) ChunkZ }, FRotator::ZeroRotator);
		LoadedChunks.Add({ ChunkX, ChunkY }, Chunk);
		ChunkGenerator Generator;
		Generator.SetWorld(this);
		Generator.Reset(x - relX, y - relY, Chunk, MinGenStage, maxGenStage, false);
		Generator.Generate();
		auto BlockData = Chunk->GetBlockData();
		return (*BlockData)[relX][relY][z];
	}*/

	return nullptr;
}

void AMcWorld::AddBlockSetTask(int32 x, int32 y, int32 z, class Block* NewBlock, uint8 MinGenStage)
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

