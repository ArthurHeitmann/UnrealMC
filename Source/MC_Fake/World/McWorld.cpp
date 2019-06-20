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
#include "Core/Public/HAL/RunnableThread.h"
#include "Kismet/GameplayStatics.h"
#include "ChunkSaveGame.h"
#include "../Misc/FileIO.h"

AMcWorld::AMcWorld()
{
	PrimaryActorTick.bCanEverTick = true;

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
	GeneratorThreads.SetNum(2);
	//GeneratorThreads.SetNum(FPlatformMisc::NumberOfCoresIncludingHyperthreads() / 2);
	//for (int i = 0; i < FPlatformMisc::NumberOfCoresIncludingHyperthreads() / 2; i++)
	for (int i = 0; i < 2; i++)
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

	DequeueGenTasks();

	bool b = false;
	if (b)
	{
		int x = 0, y = 0;
		CurrentlyLoadedChunks[{x, y}]->Destroy();
	}
}

void AMcWorld::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	UE_LOG(LogTemp, Warning, TEXT("Destroying McWorld!"));
	for (int i = 0; i < GeneratorThreads.Num(); i++)
	{
		GeneratorThreads[i]->bRun = false;
		GeneratorThreads[i]->ThisThread->Suspend(true);
		GeneratorThreads[i]->ThisThread->Kill(false);
		delete GeneratorThreads[i];
	}
	GeneratorThreads.Empty();
}

void AMcWorld::DequeueGenTasks()
{
	if (!ChunkGenBuffer.IsEmpty())
	{
		for (ChunkGenerator* Thread : GeneratorThreads)
		{
			if (Thread->bIsDone)
			{
				ChunkGenBufferElement Element;
				ChunkGenBuffer.Dequeue(Element);
				Thread->Reset(Element.x, Element.y, Element.Chunk, Element.NextGenStage);
				DequeueGenTasks();
				return;
			}
		}
	}
}

void AMcWorld::CompleteBlockSetTasks(AChunk* Chunk, int32 ChunkX, int32 ChunkY)
{
	FIntPoint key = { ChunkX, ChunkY };
	if (!BlockSetTasks.Contains(key))
		return;

	auto data = Chunk->GetChunkBlockData();
	int ChunkNextGenStage = Chunk->GetNextGenerationStage();
	bool bDataChanged = false;
	auto& Tasks = BlockSetTasks[key];
	while (Tasks.Num())
	{
		if (Tasks[0].MinGenStage <= ChunkNextGenStage)
		{
			(*data)[Tasks[0].RelX][Tasks[0].RelY][Tasks[0].z] = Tasks[0].Block;
			bDataChanged = true;
			Tasks.RemoveAtSwap(0);
		}
	}
	if (bDataChanged)
		Chunk->SetHasDataChanged();

}

void AMcWorld::FinalizeChunkGen(AChunk* FinishedChunk, int32 ChunkX, int32 ChunkY)
{
	FIntPoint key(ChunkX, ChunkY);
	if (NeighbourUpdateTasks.Contains(FinishedChunk))
	{
		ChunkNeighbourUpdates& updates = NeighbourUpdateTasks[FinishedChunk];
		if (updates.NewNChunk)
		{
			FinishedChunk->SetNorthChunk(updates.NewNChunk);
			updates.NewNChunk->SetSouthChunk(FinishedChunk);
			updates.NewNChunk = nullptr;
		}
		if (updates.NewEChunk)
		{
			FinishedChunk->SetEastChunk(updates.NewEChunk);
			updates.NewEChunk->SetWestChunk(FinishedChunk);
			updates.NewEChunk = nullptr;
		}
		if (updates.NewSChunk)
		{
			FinishedChunk->SetSouthChunk(updates.NewSChunk);
			updates.NewSChunk->SetNorthChunk(FinishedChunk);
			updates.NewSChunk = nullptr;
		}
		if (updates.NewWChunk)
		{
			FinishedChunk->SetWestChunk(updates.NewWChunk);
			updates.NewWChunk->SetEastChunk(FinishedChunk);
			updates.NewWChunk = nullptr;
		}

		NeighbourUpdateTasks.Remove(FinishedChunk);
	}

	key = { ChunkX - 1, ChunkY };
	if (CurrentlyLoadedChunks.Contains(key)) 
	{
		if (CurrentlyLoadedChunks[key]->GetNextGenerationStage() == 255)
		{
			CurrentlyLoadedChunks[key]->SetNorthChunk(FinishedChunk);
			FinishedChunk->SetSouthChunk(CurrentlyLoadedChunks[key]);
		}
		else
			NeighbourUpdateTasks.Add(CurrentlyLoadedChunks[key], { FinishedChunk, 0, 0, 0 });
	}
	key = { ChunkX + 1, ChunkY };
	if (CurrentlyLoadedChunks.Contains(key))
	{
		if (CurrentlyLoadedChunks[key]->GetNextGenerationStage() == 255)
		{
			CurrentlyLoadedChunks[key]->SetSouthChunk(FinishedChunk);
			FinishedChunk->SetNorthChunk(CurrentlyLoadedChunks[key]);
		}
		else
			NeighbourUpdateTasks.Add(CurrentlyLoadedChunks[key], { 0, 0, FinishedChunk, 0 });
	}
	key = { ChunkX, ChunkY - 1 };
	if (CurrentlyLoadedChunks.Contains(key))
	{
		if (CurrentlyLoadedChunks[key]->GetNextGenerationStage() == 255)
		{
			CurrentlyLoadedChunks[key]->SetEastChunk(FinishedChunk);
			FinishedChunk->SetWestChunk(CurrentlyLoadedChunks[key]);
		}
		else
			NeighbourUpdateTasks.Add(CurrentlyLoadedChunks[key], { 0, FinishedChunk, 0, 0 });
	}
	key = { ChunkX, ChunkY + 1 };
	if (CurrentlyLoadedChunks.Contains(key))
	{
		if (CurrentlyLoadedChunks[key]->GetNextGenerationStage() == 255)
		{
			CurrentlyLoadedChunks[key]->SetWestChunk(FinishedChunk);
			FinishedChunk->SetEastChunk(CurrentlyLoadedChunks[key]);
		}
		else
			NeighbourUpdateTasks.Add(CurrentlyLoadedChunks[key], { 0, 0, 0, FinishedChunk });
	}
}

Block* AMcWorld::GetBlock(TEnumAsByte<EAllBlocks> Block)
{
	switch (Block)
	{
	case Air:
		return B_Air::AirRef;
	case Stone:
		return new B_Stone();
	case Dirt:
		return new B_Dirt();
	case Grass:
		return new B_Grass();
	case Water:
		return new B_Water();
	case Leaves_Oak:
		return new B_Leaves();
	case Log_Oak:
		return new B_LogOak();
	}

	return B_Air::AirRef;
}

AChunk* AMcWorld::GetChunkAt(FIntPoint Location)
{
	if (CurrentlyLoadedChunks.Contains(Location))
		return CurrentlyLoadedChunks[Location];
	else
		return nullptr;
}

AChunk* AMcWorld::GetChunkAt(FVector2D Location)
{
	return GetChunkAt(FIntPoint{ (int32) Location.X, (int32) Location.Y });
}

AChunk* AMcWorld::SpawnChunk(FVector2D Location)
{
	if (AChunk * TmpChunk = GetChunkAt(Location / 1600))
		return TmpChunk;
	AChunk* Chunk;
	if (FileIO::DoesChunkExist("Debug_World", Location.X / 1600, Location.Y / 1600))
	{
		ChunkData LoadedData(FileIO::LoadChunk("Debug_World", Location.X / 1600, Location.Y / 1600));
		//TODO check for same generator version
		Chunk = GetWorld()->SpawnActor<AChunk>({Location.X, Location.Y, 0.f}, FRotator::ZeroRotator);
		Chunk->SetData(LoadedData.BlockData, true);
		Chunk->SetNextGenerationStage(LoadedData.NextGenerationStage);
		Chunk->SetLastTimeUpdated(LoadedData.LastTimeUpdated);
		if (LoadedData.NextGenerationStage != 255)
			ChunkGenBuffer.Enqueue({ ( int) Location.X / 100, ( int) Location.Y / 100, Chunk, LoadedData.NextGenerationStage });
		else
		{
			int32 ChunkX = Location.X / 1600;
			int32 ChunkY = Location.Y / 1600;
			CompleteBlockSetTasks(Chunk, ChunkX, ChunkY);
		}
	}
	else
	{
		Chunk = GetWorld()->SpawnActor<AChunk>({Location.X, Location.Y, 0.f}, FRotator::ZeroRotator);
		ChunkGenBuffer.Enqueue({ (int) Location.X / 100, (int) Location.Y / 100, Chunk, 0 }); 
	}
	
	CurrentlyLoadedChunks.Add({ (int32) Location.X / 1600, (int32) Location.Y / 1600 }, Chunk);
	return Chunk;
}

void AMcWorld::RemoveLoadedChunk(AChunk* Chunk)
{
	for (const auto& ChunkPair : CurrentlyLoadedChunks)
	{
		if (ChunkPair.Value == Chunk)
		{
			CurrentlyLoadedChunks.Remove(ChunkPair.Key);
			return;
		}
	}
}

void AMcWorld::QuickSave()
{
	for (auto ChunkPair : CurrentlyLoadedChunks)
	{
		if (ChunkPair.Value)
			FileIO::SaveChunk(ChunkPair.Value, "Debug_World");
	}
}

void AMcWorld::QuickLoad()
{
	TArray<AActor*> AllChunks;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AChunk::StaticClass(), AllChunks);
	for (AActor* ChunkA : AllChunks)
	{
		if (AChunk* Chunk = Cast<AChunk>(ChunkA))
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
	}
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
//		AChunk* c = CurrentlyLoadedChunks[{ ChunkX, ChunkY }];
//		auto data = c->GetChunkBlockData();
//		(*data)[relX][relY][z] = Block;
//		c->SetHasDataChanged();
//		return true;
//	}
//	else if (ForcePlacement && FileIO::DoesChunkExist("Debug_World", ChunkX, ChunkY))
//	{
//		ChunkData data = FileIO::LoadChunk("Debug_World", ChunkX, ChunkY);		//TODO check GenVersion
//		AChunk* Chunk = GetWorld()->SpawnActor<AChunk>({ (float) (x - relX) * 100, (float) (y - relY) * 100, 0 }, FRotator::ZeroRotator);
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
//		AChunk* Chunk = GetWorld()->SpawnActor<AChunk>({ (float) (x - relX) * 100, (float) (y - relY) * 100, 0 }, FRotator::ZeroRotator);
//		CurrentlyLoadedChunks.Add({ ChunkX, ChunkY}, Chunk);
//		ChunkGenBuffer.Enqueue({ x - relX, y - relY, Chunk, 0});
//		ChunkGenBufferArray.Add({ x - relX, y - relY });
//		return false;
//	}
//	return false;
//}

Block* AMcWorld::GetBlockAt(int32 x, int32 y, int32 z, bool ForceSuccess, int MinGenStage, int maxGenStage)
{
	//int32 ChunkX = x / 16;
	//int32 ChunkY = y / 16;
	//int32 relX = x % 16;
	//int32 relY = y % 16;
	//if (relX < 0)
	//	relX += 16;
	//if (relY < 0)
	//	relY += 16;
	//if (CurrentlyLoadedChunks.Contains({ ChunkX, ChunkY }))
	//{
	//	auto data = CurrentlyLoadedChunks[{ ChunkX, ChunkY }]->GetChunkBlockData();
	//	return (*data)[relX][relY][z];
	//}
	//else if (ForceSuccess && FileIO::DoesChunkExist("Debug_World", ChunkX, ChunkY))
	//{
	//	ChunkData data = FileIO::LoadChunk("Debug_World", ChunkX, ChunkY);		//TODO check GenVersion
	//	AChunk* Chunk = GetWorld()->SpawnActor<AChunk>({ (float) x - relX, (float) y - relY, 0 }, FRotator::ZeroRotator);
	//	CurrentlyLoadedChunks.Add({ ChunkX, ChunkY }, Chunk);
	//	Chunk->SetData(data.BlockData, true);
	//	Chunk->SetNextGenerationStage(data.NextGenerationStage);
	//	Chunk->SetLastTimeUpdated(data.LastTimeUpdated);
	//	ChunkGenerator Generator;
	//	Generator.SetWorld(this);
	//	Generator.Reset(x - relX, y - relY, Chunk, data.NextGenerationStage, maxGenStage, false);
	//	Generator.Generate();
	//	auto BlockData = Chunk->GetChunkBlockData();
	//	return (*BlockData)[relX][relY][z];
	//}
	//else if (ForceSuccess)
	//{
	//	AChunk* Chunk = GetWorld()->SpawnActor<AChunk>({ (float) x - relX, (float) y - relY, 0 }, FRotator::ZeroRotator);
	//	CurrentlyLoadedChunks.Add({ ChunkX, ChunkY }, Chunk);
	//	ChunkGenerator Generator;
	//	Generator.SetWorld(this);
	//	Generator.Reset(x - relX, y - relY, Chunk, MinGenStage, maxGenStage, false);
	//	Generator.Generate();
	//	auto BlockData = Chunk->GetChunkBlockData();
	//	return (*BlockData)[relX][relY][z];
	//}

	return B_Air::AirRef;
}

void AMcWorld::AddBlockSetTask(int32 x, int32 y, int32 z, class Block* Block, uint8 MinGenStage)
{
	BlockSetBufferElement e(x, y, z, Block, MinGenStage);
	FIntPoint key(e.ChunkX, e.ChunkY);
	if (CurrentlyLoadedChunks.Contains(key) && CurrentlyLoadedChunks[key]->GetNextGenerationStage() >= MinGenStage)
	{
		auto BlockData = CurrentlyLoadedChunks[key]->GetChunkBlockData();
		(*BlockData)[e.RelX][e.RelY][z] = Block;
		CurrentlyLoadedChunks[key]->SetHasDataChanged(true);
	}
	else
	{
		if (!BlockSetTasks.Contains(key))
			BlockSetTasks.Add(key, TArray<BlockSetBufferElement>());
		BlockSetTasks[key].Add(e);
	}
}

