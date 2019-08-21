


#include "ItemMeshComponent.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/StaticMeshComponent.h"
#include "Item.h"

void UItemMeshComponent::OnItemChange()
{
	PreviousItem = (*ItemPointer)->ItemS;
	UMaterialInstanceDynamic* TmpMaterial = UMaterialInstanceDynamic::Create(ItemMaterial, this);
	TmpMaterial->SetTextureParameterValue(TEXT("Item Material"), (*ItemPointer)->ItemS->GetTexture());
	StaticMesh->SetMaterial(0, TmpMaterial);
}

void UItemMeshComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunc)
{
	if (PreviousItem != (*ItemPointer)->ItemS)
	{
		PreviousItem = (*ItemPointer)->ItemS;
		OnItemChange();
	}
}

UItemMeshComponent::UItemMeshComponent() {
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ItemMeshFinder(TEXT("StaticMesh'/Game/Meshes/Item/AutoItem_16px.AutoItem_16px'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> ItemMaterialFinder(TEXT("Material'/Game/Materials/Items/ItemMaterial.ItemMaterial'"));
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StatMesh"));
	StaticMesh->SetStaticMesh(ItemMeshFinder.Object);
	SetCollisionProfileName(TEXT("NoCollision"));
	ItemMaterial = ItemMaterialFinder.Object;
}

void UItemMeshComponent::SetItem(FItemStack** NewItemStackPointer)
{
	ItemPointer = NewItemStackPointer;
	OnItemChange();
}
