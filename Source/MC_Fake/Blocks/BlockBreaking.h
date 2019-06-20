

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlockBreaking.generated.h"

UCLASS()
class MC_FAKE_API ABlockBreaking : public AActor
{
	GENERATED_BODY()
	
private:
	class Block* BlockData;
	class UProceduralMeshComponent* Mesh;
	class UProceduralMeshComponent* CollisionMesh;
	UPROPERTY(EditAnywhere)
	USceneComponent* Root;

protected:
	virtual void BeginPlay() override;

public:	
	ABlockBreaking();

	void InitWithBlock(class Block* NewBlock);
	void SetBreakingStage(float percentage);

	class Block* GetBlock();
};
