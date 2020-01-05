

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlockBreaking.generated.h"

UCLASS()
/**
 * If a player starts breaking a block, the block will be temporarily removed and this Actor
 * will be placed in its location.
 * This Actor is an indicator for the progress of breaking.
 */
class MC_FAKE_API ABlockBreaking : public AActor
{
	GENERATED_BODY()
	
private:
	/** The block that this Actor represents */
	class B_Block* BlockReference;
	/** Visible Mesh to the Player */
	class URuntimeMeshComponent* Mesh;
	/**
	 * This Mesh is used for all collisions with this Actor.
	 * Has the shape of the original mesh.
	 * For Example: The visible mesh has shrunken, but the hit box should be of the original mesh.
	 */
	class URuntimeMeshComponent* CollisionMesh;
	UPROPERTY(EditAnywhere)
	USceneComponent* Root;

protected:
	virtual void BeginPlay() override;

public:	
	ABlockBreaking();

	/** Creates the visible & collision mesh */
	void InitWithBlock(class B_Block* NewBlock);
	/** Adjust the visualizer to the new progress */
	void SetTimePassed(float Seconds);

	/** @return The Block that this Actor represents */
	class B_Block* GetBlock();
};
