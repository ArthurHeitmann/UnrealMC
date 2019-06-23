


#include "ItemSystemComponent.h"

UItemSystemComponent::UItemSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UItemSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	
}


// Called every frame
void UItemSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

