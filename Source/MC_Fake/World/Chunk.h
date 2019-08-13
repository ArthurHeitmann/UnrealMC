#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Blocks/Block.h"
#include "Chunk.generated.h"

UCLASS()
class MC_FAKE_API AChunk : public AActor
{
	GENERATED_BODY()

	struct BlockBreakingData {
		class ABlockBreaking* Block;
		float HitTime;
		class Item* ItemUsed;
		int x; int y; int z;

		bool operator==(BlockBreakingData b)
		{
			return Block == b.Block && x == b.x && y == b.y && z == b.z;
		}
	};

public:	

	AChunk();
	UFUNCTION()
		void ChunkEntered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void SetData(const TArray<TArray<TArray<Block*>>>& NewData, bool bUpdateMesh);
	void SetHasDataChanged(bool state = true);
	void SetHasFinishedGenerating(bool state);
	void UpdateMesh();
	void SetMeshLifeStage(int Stage);
	Block* RegisterHitAt(const FHitResult& HitResult, class Item* Item);
	bool ContinueHit(class ABlockBreaking* Block, class Item* Item);
	void CancelBreaking(class Block* Block);
	int GetNextGenerationStage();
	void SetNextGenerationStage(int NewStage);
	TArray<TArray<TArray<Block*>>>* GetChunkBlockData();
	Block* GetBlockAt(int x, int y, int z);
	void SetNorthChunk(AChunk* c);
	void SetEastChunk(AChunk* c);
	void SetSouthChunk(AChunk* c);
	void SetWestChunk(AChunk* c);

	uint64 GetLastTimeUpdated();
	void SetLastTimeUpdated(float NewTime);
	

	virtual void Tick(float Delta) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type Reason) override;

private:
	UPROPERTY(EditAnywhere)
	USceneComponent* Root;
	class UBoxComponent* ChunkEnterTriggerBox;
	UPROPERTY(EditAnywhere)
	class URuntimeMeshComponent* ChunkMesh;
	//class UProceduralMeshComponent* ChunkMesh;
	TArray<TArray<TArray<Block*>>> ChunkBlockData;
	int NextGenerationStage = 0;
	class AMcWorld* McFWorld;
	int32 PosX, PosY;
	int LifeStage = 0;		//0: normal (visible), 1: Only shadows; no Mesh, 2: Hidden, 3: to be destroyed
	bool bHasDataChanged = false;
	bool bHasFinishedGenerating = false;
	double LastTimeUpdated = 0;
	AChunk* NorthChunk;
	AChunk* EastChunk;
	AChunk* SouthChunk;
	AChunk* WestChunk;


	TArray<BlockBreakingData> BreakingBlocks;

	
	bool ShouldFaceBePlacedBetween(Block* b1, Block* b2, TEnumAsByte<EFaceDirection> Side);
};
