#include "Character_Boxed.h"

#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "GameFramework/CharacterMovementComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogCharacter, Log, All);

FName ACharacter_Boxed::BoxComponentName(TEXT("CollisionBox"));

// Sets default values
ACharacter_Boxed::ACharacter_Boxed(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(ACharacter_Boxed::BoxComponentName);
	BoxComponent->SetBoxExtent({ 34.f, 34.f, 88.f });
	BoxComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);

	BoxComponent->CanCharacterStepUpOn = ECB_No;
	BoxComponent->SetShouldUpdatePhysicsVolume(true);
	BoxComponent->SetCanEverAffectNavigation(false);
	BoxComponent->bDynamicObstacle = true;
	RootComponent = BoxComponent;
}

void ACharacter_Boxed::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!IsPendingKill())
	{
		if (GetMesh())
		{
			CacheInitialMeshOffset(GetMesh()->RelativeLocation, GetMesh()->RelativeRotation);

			// force animation tick after movement component updates
			if (GetMesh()->PrimaryComponentTick.bCanEverTick && GetCharacterMovement())
			{
				GetMesh()->PrimaryComponentTick.AddPrerequisite(GetCharacterMovement(), GetCharacterMovement()->PrimaryComponentTick);
			}
		}

		if (GetCharacterMovement() && BoxComponent)
		{
			GetCharacterMovement()->UpdateNavAgent(*this);		// TODO check if correct
		}

		if (Controller == nullptr && GetNetMode() != NM_Client)
		{
			if (GetCharacterMovement() && GetCharacterMovement()->bRunPhysicsWithNoController)
			{
				GetCharacterMovement()->SetDefaultMovementMode();
			}
		}
	}
}

void ACharacter_Boxed::GetSimpleCollisionCylinder(float& CollisionRadius, float& CollisionHalfHeight) const
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	if (IsTemplate())
	{
		UE_LOG(LogCharacter, Log, TEXT("WARNING ACharacter_Boxed::GetSimpleCollisionCylinder : Called on default object '%s'. Will likely return zero size. Consider using GetDefaultHalfHeight() instead."), *this->GetPathName());
	}
#endif

	if (RootComponent == BoxComponent && IsRootComponentCollisionRegistered())
	{
		// Note: we purposefully ignore the component transform here aside from scale, always treating it as vertically aligned.
		// This improves performance and is also how we stated the BoxComponent would be used.
		CollisionHalfHeight = GetScaledBoxHalfHeight(BoxComponent);
		CollisionRadius = GetScaledBoxAverageRadius(BoxComponent);
	}
	else
	{
		Super::GetSimpleCollisionCylinder(CollisionRadius, CollisionHalfHeight);
	}
}

void ACharacter_Boxed::UpdateNavigationRelevance()
{
	if (BoxComponent)
	{
		BoxComponent->SetCanEverAffectNavigation(bCanAffectNavigationGeneration);
	}
}

float ACharacter_Boxed::GetDefaultHalfHeight() const
{
	UBoxComponent* DefaultCapsule = GetClass()->GetDefaultObject<ACharacter_Boxed>()->BoxComponent;
	if (DefaultCapsule)
	{
		return GetScaledBoxHalfHeight(DefaultCapsule);
	}
	else
	{
		return Super::GetDefaultHalfHeight();
	}
}

FVector ACharacter_Boxed::GetNavAgentLocation() const
{
	FVector AgentLocation = FNavigationSystem::InvalidLocation;

	if (GetCharacterMovement() != nullptr)
	{
		AgentLocation = GetCharacterMovement()->GetActorFeetLocation();
	}

	if (FNavigationSystem::IsValidLocation(AgentLocation) == false && BoxComponent != nullptr)
	{
		AgentLocation = GetActorLocation() - FVector(0, 0, GetScaledBoxHalfHeight(BoxComponent));
	}

	return AgentLocation;
}

float ACharacter_Boxed::GetScaledBoxHalfHeight(UBoxComponent* Box) const
{
	return Box->GetScaledBoxExtent().Z;
}

float ACharacter_Boxed::GetScaledBoxAverageRadius(UBoxComponent* Box) const
{
	return (Box->GetScaledBoxExtent().X + Box->GetScaledBoxExtent().Y) / 2.0;
}
