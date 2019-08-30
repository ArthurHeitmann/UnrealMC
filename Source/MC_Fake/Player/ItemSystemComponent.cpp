


#include "ItemSystemComponent.h"
#include "ItemSlots.h"
#include "QuickAccesSlots.h"
#include "../Items/ItemDrop.h"
#include "../Items/ItemStack.h"
#include "Components/BoxComponent.h"
#include "../Items/ItemMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

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


	Slot_QuickAcces->UpdateSlotsUI(GetWorld());
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

void UItemSystemComponent::InitSelectedItemMesh(USceneComponent* AttachTo, const FTransform & Offset1, const FTransform & Offset2)
{
	SelectedItemMesh = NewObject<UItemMeshComponent>(this, TEXT("Selected Item Mesh Component"));
	SelectedItemMesh->SetupAttachment(AttachTo);
	SelectedItemMesh->RegisterComponent();
	SelectedItemMesh->SetItemMeshOffset(Offset1);
	SelectedItemMesh->SetCustomMeshOffset(Offset2);
}

void UItemSystemComponent::InitQuickAccessSlots(int32 Num)
{
	if (Slot_QuickAcces)
		return;

	Slot_QuickAcces = NewObject<UQuickAccesSlots>(this);
	Slot_QuickAcces->SetNumSlots(Num);
}

void UItemSystemComponent::SelectItemSlot(int32 num)
{
	FItemStack& ItemS = Slot_QuickAcces->SetSelecteddItemSlotById(num);
	*SelectedItemRef = &ItemS;
}

void UItemSystemComponent::SelectNextItem()
{
	FItemStack& ItemS = Slot_QuickAcces->NextSelectedItem();
	*SelectedItemRef = &ItemS;
}

void UItemSystemComponent::SelectPreviousItem()
{
	FItemStack& ItemS = Slot_QuickAcces->PreviousSelectedItem();
	*SelectedItemRef = &ItemS;
}

void UItemSystemComponent::SetSelectedItemPointer(FItemStack** NewISPointer)
{
	SelectedItemRef = NewISPointer;
	if (Slot_QuickAcces)
	{
		FItemStack& FISR = Slot_QuickAcces->GetStackAt(0);
		*SelectedItemRef = &FISR;
	}
	if (SelectedItemMesh)
		SelectedItemMesh->SetItem(SelectedItemRef);
}

void UItemSystemComponent::ItemPickBoxTrigger(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AItemDrop * Drop = Cast<AItemDrop>(OtherActor))
	{
		int32 ItemsLeft = AddItemStackToInventory(Drop->GetItemStack())
			.ItemCount;
		Drop->UpdateItemCount(ItemsLeft, UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetRootComponent(), {0, 0, -30});
	}
}
