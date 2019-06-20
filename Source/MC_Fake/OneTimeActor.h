

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OneTimeActor.generated.h"

UCLASS()
class MC_FAKE_API AOneTimeActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AOneTimeActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
