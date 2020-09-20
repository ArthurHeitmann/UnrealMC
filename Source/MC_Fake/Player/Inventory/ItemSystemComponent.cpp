


#include "ItemSystemComponent.h"
#include "ItemSlots.h"
#include "../../UI/UI_QuickAccessSlots.h"
#include "../../Items/ItemDrop.h"
#include "../../Items/ItemStack.h"
#include "Components/BoxComponent.h"
#include "../../Items/ItemMeshComponent.h"
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


void UItemSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

FItemStack UItemSystemComponent::AddItemStackToInventory(FItemStack Items)
{
	if (!QuickAccessSlots)
		return Items;

	return Slot_BasicInventory->PickupItemStack(Items);
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

void UItemSystemComponent::InitSelectedItemMesh(USceneComponent* AttachTo)
{
	FTransform ItemMeshTrans;
	FTransform BlockMeshTrans;

	ItemMeshTrans.SetLocation(FVector{ 38, 45, -45 });
	ItemMeshTrans.SetRotation(FRotator(0, 90, 0).Quaternion());
	ItemMeshTrans.SetScale3D(FVector{ 0.9, 0.9, 0.9 });
	BlockMeshTrans.SetLocation(FVector{ 27, 28, -34 });
	BlockMeshTrans.SetRotation(FRotator(0, 315, 0).Quaternion());
	BlockMeshTrans.SetScale3D(FVector{ 0.25, 0.25, 0.25 });

	InitSelectedItemMesh(AttachTo, ItemMeshTrans, BlockMeshTrans);
}

void UItemSystemComponent::InitSelectedItemMesh(USceneComponent* AttachTo, const FTransform & Offset1, const FTransform & Offset2)
{
	auto a = Offset2.GetLocation();
	
	SelectedItemMesh = NewObject<UItemMeshComponent>(this, TEXT("Selected Item Mesh Component"));
	SelectedItemMesh->SetupAttachment(AttachTo);
	SelectedItemMesh->RegisterComponent();
	SelectedItemMesh->SetItemMeshOffset(Offset1);
	SelectedItemMesh->SetCustomMeshOffset(Offset2);
}

void UItemSystemComponent::InitItemSlots(int32 Num)
{
	if (Slot_BasicInventory)
		return;

	Slot_BasicInventory = NewObject<UItemSlots>(this);
	Slot_BasicInventory->SetNumSlots(Num);
}

void UItemSystemComponent::InitUI(AGameModeBase * Gamemode)
{
	if (!Slot_BasicInventory && QuickAccessSlots)
		return;
	//Check if getWorld works and Remove Gamemode Parameter
	QuickAccessSlots = CreateWidget<UUI_QuickAccessSlots>(GetWorld(), UUI_QuickAccessSlots::StaticClass(), TEXT("Quick Access Slots"));
	QuickAccessSlots->SetInventorySlots(Slot_BasicInventory, 10);
	if (SelectedItemRef)
		QuickAccessSlots->SetSelectedItemPointer(SelectedItemRef);
	QuickAccessSlots->AddToViewport();
}

void UItemSystemComponent::SelectItemSlot(int32 num)
{
	QuickAccessSlots->SelectItemStack(num);
}

void UItemSystemComponent::SelectNextItem()
{
	if (QuickAccessSlots)
		QuickAccessSlots->SelectNextItem();
}

void UItemSystemComponent::SelectPreviousItem()
{
	if (QuickAccessSlots)
		QuickAccessSlots->SelectPreviousItem();
}

void UItemSystemComponent::SetSelectedItemPointer(FItemStack const ** NewISPointer)
{
	SelectedItemRef = NewISPointer;
	if (QuickAccessSlots)
	{
		FItemStack* FISR = Slot_BasicInventory->GetStackAt(0);
		*SelectedItemRef = FISR;
	}
	if (SelectedItemMesh)
		SelectedItemMesh->SetItem(SelectedItemRef);
}

void UItemSystemComponent::SetItemWheelVisibility(bool State)
{
	QuickAccessSlots->SetWheelVisibility(State);
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
