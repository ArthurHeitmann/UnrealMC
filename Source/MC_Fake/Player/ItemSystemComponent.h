

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemSystemComponent.generated.h"

struct ItemStack {
	class Item* Item;
	int32 Count;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MC_FAKE_API UItemSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UItemSystemComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

private:	
	ItemStack* CurrentItem;
		
};
