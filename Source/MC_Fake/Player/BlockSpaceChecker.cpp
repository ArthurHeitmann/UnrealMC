#include "BlockSpaceChecker.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"

ABlockSpaceChecker::ABlockSpaceChecker()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>("Collision Box");
	SetRootComponent(CollisionBox);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Overlap);
	CollisionBox->SetBoxExtent({ 30.f, 30.f, 30.f });
	CollisionBox->bHiddenInGame = false;
	CollisionBox->SetRelativeLocation({ 50.f, 50.f, 30.f });
}

void ABlockSpaceChecker::BeginPlay()
{
	Super::BeginPlay();

	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ABlockSpaceChecker::OnOverlap);
}

void ABlockSpaceChecker::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<APawn>(OtherActor) && Cast<UCapsuleComponent>(OtherComp))
		bHitPlayer = true;
}
