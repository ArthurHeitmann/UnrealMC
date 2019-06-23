


#include "ItemMeshComponent.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Item.h"

UStaticMesh* UItemMeshComponent::ItemMesh = nullptr;
UMaterial* UItemMeshComponent::ItemMaterial = nullptr;

UItemMeshComponent::UItemMeshComponent() {
	if (!ItemMesh)
	{
		static ConstructorHelpers::FObjectFinder<UStaticMesh> ItemMeshFinder(TEXT("StaticMesh'/Game/Meshes/Item/AutoItem_16px.AutoItem_16px'"));
		static ConstructorHelpers::FObjectFinder<UMaterial> ItemMaterialFinder(TEXT("Material'/Game/Materials/Items/ItemMaterial.ItemMaterial'"));
		ItemMesh = ItemMeshFinder.Object;
		ItemMaterial = ItemMaterialFinder.Object;
	}
	SetStaticMesh(ItemMesh);
	SetCollisionProfileName(TEXT("NoCollision"));
}

void UItemMeshComponent::InitWithItem(Item* Item)
{
	UMaterialInstanceDynamic* TmpMaterial = UMaterialInstanceDynamic::Create(ItemMaterial, this);
	TmpMaterial->SetTextureParameterValue(TEXT("Item Material"), Item->GetTexture());
	SetMaterial(0, TmpMaterial);
}
