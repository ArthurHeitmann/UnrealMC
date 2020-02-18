

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Items/ItemStack.h"
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
	class ULineTracingInteractions* LineTracing;
	UPROPERTY(EditAnywhere)
	class UItemSystemComponent* ItemSystem;
	UPROPERTY()
	class UI_PauseMenu* PauseMenu;
	
	FItemStack* CurrentItem;

	void MoveForward(float v);
	void MoveRight(float v);
	void LookUp(float v);
	void Turn(float v);
	void LeftClick();
	void LeftClickEnd();
	void RightClick();
	void RightClickEnd();
	void ToggleItemWheel(bool State);
	DECLARE_DELEGATE_OneParam(FToggleItemWheel, bool);
	void ScrollUp();
	void ScrollDown();
	void Save();
	void Load();
	void SelectItem(int32 numb);
	DECLARE_DELEGATE_OneParam(FSelectItemDelegate, int32);
	void StartCrouch();
	void EndCrouch();
	void ToggleChunkBoarders();
	UFUNCTION(BlueprintCallable)
	void TogglePauseMenu();

protected:
	virtual void BeginPlay() override;

public:	
	ADebugQuickCharacter();
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UWorldLoadingComponent* GetWorldLoadingComponent();
};
