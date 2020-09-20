#include "Chunk.h"
#include "Components/BoxComponent.h"
#include "McWorld.h"
#include "GameFramework/Character.h"
#include "ChunkCube.h"
#include "ChunkFormCoords.h"


UChunk::UChunk()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UChunk::BeginPlay()
{
	Super::BeginPlay();
}

void UChunk::Init(FChunkFormCoords2D pPos, AMcWorld* pMcWorld)
{
	Pos = pPos;
	McWorld = pMcWorld;
}

void UChunk::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	bHasBeenDestroyed = true;
	Super::EndPlay(EndPlayReason);
	
	while (bIsGenerating)
		FPlatformProcess::Sleep(0.01);

	McWorld->RemoveLoadedChunk(Pos);

	if (!bHasFinishedGenerating)
		McWorld->AddRemovedChunk(this);

	for (auto ccube : ChunkCubes)
	{
		ccube.Value->DestroyComponent();
	}
	
	// TArray<USceneComponent*> Comps;
	// Root->GetChildrenComponents(true, Comps);
	// for (auto& Comp : Comps) {
	// 	Comp->DestroyComponent();
	// }
	//
	// Root->DestroyComponent();
	//
	// for (auto& Cube : ChunkCubes)
	// 	delete Cube.Value;
}

void UChunk::CreateChunkCube(int8 PosZ)
{
 	// ChunkCube* NewCube = new ChunkCube({ Pos.X, Pos.Y, PosZ }, McWorld, this);
	UChunkCube* NewCube = NewObject<UChunkCube>(this);
	NewCube->SetupAttachment(this);
	NewCube->RegisterComponent();
	NewCube->AddLocalOffset({ 0, 0, PosZ * 1600.f });
	NewCube->Init( FChunkFormCoords3D{ Pos.X, Pos.Y, PosZ }, McWorld, this );
	ChunkCubes.Add(PosZ, NewCube);
	McWorld->AddLoadedChunkCube(NewCube, NewCube->GetPos());
	McWorld->AddChunkGenTask(NewCube);
}

void UChunk::SetHasFinishedGenerating(bool bState = true)
{
	bHasFinishedGenerating = bState;
}

bool UChunk::GetHasFinishedGenerating()
{
	return bHasFinishedGenerating;
}

bool UChunk::GetIsGenerating()
{
	return bIsGenerating;
}

void UChunk::SetIsGenerating(bool bState)
{
	bIsGenerating = bState;
}

UChunkCube* UChunk::GetChunkCube(int8 PosZ)
{
	UChunkCube** found = ChunkCubes.Find(PosZ);
	return found ? *found : nullptr;
}

void UChunk::UpdateChunkCubesLoading(int8 BaseHeight, int8 RangeDown, int8 RangeUp)
{
	for (int8 z = -RangeDown; z <= RangeUp; ++z)
	{
		int8 PosZ = BaseHeight + z;
		if (PosZ >= 0 && PosZ < 16 && !ChunkCubes.Contains(PosZ))
			CreateChunkCube(PosZ);
	}
}

TMap<int8, class UChunkCube*>& UChunk::GetChunkCubes()
{
	return ChunkCubes;
}

B_Block* UChunk::GetBlockAt(int x, int y, int z)
{
	int8 Key = z / 16;
	if (ChunkCubes.Contains(Key))
		return (*ChunkCubes.Find(Key))->GetBlockAt(x, y, Key);
	else
		return nullptr;
}

B_Block*& UChunk::GetBlockAtAsRef(int x, int y, int z)
{
	int8 Key = z / 16;
	return (*ChunkCubes.Find(Key))->GetBlockAt(x, y, Key);
}

ChunkGenMaps& UChunk::GetChunkGenMaps()
{
	return ChunkDataMaps;
}

bool UChunk::HasBeenDestroyed()
{
	return bHasBeenDestroyed;
}

void UChunk::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

FChunkFormCoords2D UChunk::GetPos()
{
	return Pos;
}

