


#include "ItemMeshComponent.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/StaticMeshComponent.h"
#include "Item.h"

void UItemMeshComponent::OnItemChange()
{
	if ((*ItemPointer)->ItemCount > 0)
	{
		if (bWasPreviouseItemEmpty)
			SetVisibility(true);

		ItemMaterial->SetTextureParameterValue(TEXT("Item Material"), (*ItemPointer)->ItemS->GetTexture());
		SetMaterial(0, ItemMaterial);
	}
	else
	{
		SetVisibility(false);
		bWasPreviouseItemEmpty = true;
	}
}

void UItemMeshComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunc)
{
	if (PreviousItem != (*(*ItemPointer)->ItemS))
	{
		OnItemChange();
		PreviousItem = *((*ItemPointer)->ItemS);
	}
}

UItemMeshComponent::UItemMeshComponent() 
{
	PrimaryComponentTick.bCanEverTick = true;
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ItemMeshFinder(TEXT("StaticMesh'/Game/Meshes/Item/AutoItem_16px.AutoItem_16px'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> ItemMaterialFinder(TEXT("Material'/Game/Materials/Items/ItemMaterial.ItemMaterial'"));
	SetStaticMesh(ItemMeshFinder.Object);
	ItemMaterial = UMaterialInstanceDynamic::Create(ItemMaterialFinder.Object, GetWorld());
	//SetMaterial(0, ItemMaterial);
	SetCollisionProfileName(TEXT("NoCollision"));
}

void UItemMeshComponent::SetItem(FItemStack** NewItemStackPointer)
{
	ItemPointer = NewItemStackPointer;
	PreviousItem = *((*ItemPointer)->ItemS);
	OnItemChange();
	
}
