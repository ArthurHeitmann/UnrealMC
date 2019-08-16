


#include "ItemSystemComponent.h"
#include "ItemSlots.h"
#include "QuickAccesSlots.h"
#include "../Items/ItemDrop.h"
#include "../Items/ItemStack.h"
#include "Components/BoxComponent.h"

UItemSystemComponent::UItemSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	/*ItemPickupBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Item Pickup Trigger"));
	ItemPickupBox->SetupAttachment(this);*/
	//ItemPickupBox->SetWorldLocation(GetComponentLocation());
	//ItemPickupBox->AddRelativeLocation({ 0, 0, 25 });
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

FItemStack UItemSystemComponent::AddItemStackToInventory(FItemStack Items)
{
	if (!Slot_QuickAcces)
		return Items;

	return Slot_QuickAcces->PickupItemStack(Items);
}

void UItemSystemComponent::InitPickUpBox(const FVector& BoxExtent)
{
	ItemPickupBox = NewObject<UBoxComponent>(this);
	ItemPickupBox->SetupAttachment(this);
	ItemPickupBox->RegisterComponent();
	ItemPickupBox->SetBoxExtent(BoxExtent);
	ItemPickupBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Ignore);
	ItemPickupBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECR_Ignore);
	ItemPickupBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECR_Ignore);
	ItemPickupBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECR_Overlap);
	ItemPickupBox->OnComponentBeginOverlap.AddDynamic(this, &UItemSystemComponent::ItemPickBoxTrigger);
}

void UItemSystemComponent::InitQuickAccessSlots(int32 Num)
{
	if (Slot_QuickAcces)
		return;

	Slot_QuickAcces = NewObject<UQuickAccesSlots>();
	Slot_QuickAcces->SetNumSlots(Num);
}

void UItemSystemComponent::ItemPickBoxTrigger(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AItemDrop * Drop = Cast<AItemDrop>(OtherActor))
	{
		int32 ItemsLeft = AddItemStackToInventory(Drop->GetItemStack())
			.ItemCount;
		Drop->UpdateItemCount(ItemsLeft);

		Slot_QuickAcces->DebugPrint(GetWorld());
	}
}
