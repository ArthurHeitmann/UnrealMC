#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChunkGenerator.h"
#include "Containers/Queue.h"
#include "Enums.h"
#include "McWorld.generated.h"


UCLASS()
class MC_FAKE_API AMcWorld : public AActor
{
	GENERATED_BODY()

	struct ChunkGenBufferElement {
		int32 x; int32 y; class AChunk* Chunk; int NextGenStage; FRunnableThread* Thread = nullptr;
	};

	struct BlockSetBufferElement {
		int32 ChunkX, ChunkY;
		int32 RelX; int32 RelY; int32 z;
		class Block* Block;
		uint8 MinGenStage;

		BlockSetBufferElement() {}
		BlockSetBufferElement(int32 x, int32 y, int32 z, class Block* b, uint8 MinGenerationStage)
		{
			RelX = x % 16;
			RelY = y % 16;
			if (RelX < 0)
				RelX += 16;
			if (RelY < 0)
				RelY += 16;
			ChunkX = x / 16;
			ChunkY = y / 16;
			if (x < 0 && RelX)
				ChunkX--;
			if (y < 0 && RelY)
				ChunkY--;
			this->z = z;

			Block = b;
			MinGenStage = MinGenerationStage;
		}
	};

	struct ChunkNeighbourUpdates {
		AChunk* NewNChunk;
		AChunk* NewEChunk;
		AChunk* NewSChunk;
		AChunk* NewWChunk;
	};

public:	
	AMcWorld();
	virtual void Tick(float DeltaTime) override;

	class Block* GetBlock(TEnumAsByte<EAllBlocks> Block);
	class AChunk* GetChunkAt(FIntPoint Location);
	class AChunk* GetChunkAt(FVector2D Location);
	class AChunk* SpawnChunk(FVector2D Location);
	void RemoveLoadedChunk(class AChunk* Chunk);
	void QuickSave();
	void QuickLoad();
	//bool SetBlockAt(int32 x, int32 y, int32 z, class Block* Block, bool ForcePlacement, int MinGenStage = 0, int maxGenStae = 255);
	class Block* GetBlockAt(int32 x, int32 y, int32 z, bool ForceSuccess, int MinGenStage = 0, int maxGenStae = 255);
	void AddBlockSetTask(int32 x, int32 y, int32 z, class Block* Block, uint8 MinGenStage);
	void CompleteBlockSetTasks(AChunk* Chunk, int32 ChunkX, int32 ChunkY);
	void FinalizeChunkGen(AChunk* FinishedChunk, int32 ChunkX, int32 ChunkY);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	TMap<FIntPoint, class UChunkCube*> LoadedChunkCubes;
	TArray<ChunkGenerator*> GeneratorThreads;
	TQueue<ChunkGenBufferElement> ChunkGenBuffer;
	TMap<FIntPoint, TArray<BlockSetBufferElement>> BlockSetTasks;
	TMap<AChunk*, ChunkNeighbourUpdates> NeighbourUpdateTasks;

	void DequeueGenTasks();
};