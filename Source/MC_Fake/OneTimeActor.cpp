


#include "OneTimeActor.h"
#include "Kismet/GameplayStatics.h"

AOneTimeActor::AOneTimeActor()
{
	PrimaryActorTick.bCanEverTick = false;

}

void AOneTimeActor::BeginPlay()
{
	Super::BeginPlay();
	TArray<AActor*> AllActors;
	if (const UWorld * W = GetWorld())
		UGameplayStatics::GetAllActorsOfClass(W, AOneTimeActor::StaticClass(), AllActors);
}

void AOneTimeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

