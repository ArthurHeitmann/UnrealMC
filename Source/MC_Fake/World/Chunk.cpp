#include "Chunk.h"
#include "Components/BoxComponent.h"
#include "Blocks/B_Stone.h"
#include "McWorld.h"
#include "GameFramework/Character.h"
#include "ChunkCube.h"
#include "ChunkFormCoords.h"

Chunk::Chunk(ChunkFormCoords2D Pos, class AMcWorld* McWorld)
{
	this->Pos = Pos;
	this->McWorld = McWorld;

	Root = NewObject<USceneComponent>(McWorld);
	Root->SetupAttachment(McWorld->ChunksRoot);
	Root->RegisterComponent();
	Root->AddWorldOffset({ Pos.X * 1600.f, Pos.Y * 1600.f, 0.f });
	CubesRoot = NewObject<USceneComponent>(McWorld);
	CubesRoot->SetupAttachment(Root);
	CubesRoot->RegisterComponent();
	CubesRoot->AddLocalOffset({ 0.f, 0.f, 300.f });
}

Chunk::~Chunk()
{
	while (bIsGenerating)
		FPlatformProcess::Sleep(0.01);

	McWorld->RemoveLoadedChunk(Pos);

	if (!bHasFinishedGenerating)
		McWorld->AddRemovedChunk(this);

	TArray<USceneComponent*> Comps;
	Root->GetChildrenComponents(true, Comps);
	for (auto& Comp : Comps)
		Comp->DestroyComponent();

	Root->DestroyComponent();

	for (auto& Cube : ChunkCubes)
		delete Cube.Value;
}

void Chunk::CreateChunkCube(int8 PosZ)
{
	ChunkCube* NewCube = new ChunkCube({ Pos.X, Pos.Y, PosZ }, McWorld, this);
	ChunkCubes.Add(PosZ, NewCube);
	McWorld->AddLoadedChunkCube(NewCube, NewCube->GetPos());
	McWorld->AddChunkGenTask(NewCube);

}

void Chunk::SetHasFinishedGenerating(bool bState = true)
{
	bHasFinishedGenerating = bState;
}

bool Chunk::GetHasFinishedGenerating()
{
	return bHasFinishedGenerating;
}

bool Chunk::GetIsGenerating()
{
	return bIsGenerating;
}

void Chunk::SetIsGenerating(bool bState)
{
	bIsGenerating = bState;
}

ChunkCube* Chunk::GetChunkCube(int8 PosZ)
{
	ChunkCube** found = ChunkCubes.Find(PosZ);
	return found ? *found : nullptr;
}

void Chunk::UpdateChunkCubesLoading(int8 BaseHeight, int8 RangeDown, int8 RangeUp)
{
	for (int8 z = -RangeDown; z <= RangeUp; ++z)
	{
		int8 PosZ = BaseHeight + z;
		if (PosZ >= 0 && PosZ < 16 && !ChunkCubes.Contains(PosZ))
			CreateChunkCube(PosZ);
	}
}

TMap<int8, class ChunkCube*>& Chunk::GetChunkCubes()
{
	return ChunkCubes;
}

B_Block* Chunk::GetBlockAt(int x, int y, int z)
{
	int8 Key = z / 16;
	if (ChunkCubes.Contains(Key))
		return (*ChunkCubes.Find(Key))->GetBlockAt(x, y, Key);
	else
		return nullptr;
}

B_Block*& Chunk::GetBlockAtAsRef(int x, int y, int z)
{
	int8 Key = z / 16;
	return (*ChunkCubes.Find(Key))->GetBlockAt(x, y, Key);
}

ChunkGenMaps& Chunk::GetChunkGenMaps()
{
	return ChunkDataMaps;
}

void Chunk::Tick(float Delta)
{
	for (auto& cube : ChunkCubes)
	{
		cube.Value->Tick(Delta);
	}
	
}

ChunkFormCoords2D Chunk::GetPos()
{
	return Pos;
}

