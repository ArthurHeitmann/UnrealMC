

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DebugQuickCharacter.generated.h"

UCLASS()
class MC_FAKE_API ADebugQuickCharacter : public ACharacter
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly)
	class UCameraComponent* Camera;
	UPROPERTY(EditAnywhere)
	class UWorldLoadingComponent* WorldLoader;
	UPROPERTY(EditAnywhere)
	class ULineTraceingInteractions* LineTracer;
	UPROPERTY(EditAnywhere)
	class UItemMeshComponent* CurrentItemMesh;
	UPROPERTY(EditAnywhere)
	class UItemSystemComponent* ItemSystem;
	
	class Item** CurrentItem;
	class Item* PreviousItem;

	void MoveForward(float v);
	void MoveRight(float v);
	void LookUp(float v);
	void Turn(float v);
	void LeftClick();
	void LeftClickEnd();
	void RightClick();
	void RightClickEnd();
	void Save();
	void Load();
	void SelectItem1();
	void SelectItem2();
	void StartCrouch();
	void EndCrouch();

protected:
	virtual void BeginPlay() override;

public:	
	ADebugQuickCharacter();
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
