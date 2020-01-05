#include "MC_FakeGameModeBase.h"
#include "DebugQuickCharacter.h"

void AMC_FakeGameModeBase::BeginPlay()
{
	Super::BeginPlay();
}

AMC_FakeGameModeBase::AMC_FakeGameModeBase()
{
	DefaultPawnClass = ADebugQuickCharacter::StaticClass();
}
