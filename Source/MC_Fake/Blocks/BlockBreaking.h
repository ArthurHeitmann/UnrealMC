

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlockBreaking.generated.h"

UCLASS()
class MC_FAKE_API ABlockBreaking : public AActor
{
	GENERATED_BODY()
	
private:
	class B_Block* BlockData;
	class URuntimeMeshComponent* Mesh;
	class URuntimeMeshComponent* CollisionMesh;
	UPROPERTY(EditAnywhere)
	USceneComponent* Root;

protected:
	virtual void BeginPlay() override;

public:	
	ABlockBreaking();

	void InitWithBlock(class B_Block* NewBlock);
	void SetTimePassed(float seconds);

	class B_Block* GetBlock();
};
