#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MC_FakeGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class MC_FAKE_API AMC_FakeGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	AMC_FakeGameModeBase();
	class UItemSlotsHUD* ItemSlotWidget;


};
