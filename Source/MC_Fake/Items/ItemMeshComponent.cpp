


#include "ItemMeshComponent.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/StaticMeshComponent.h"
#include "I_Item.h"
#include "RuntimeMeshProviderStatic.h"

void UItemMeshComponent::OnItemChange()
{
	if ((*ItemPointer)->ItemCount > 0)
	{
		bool bCustomMesh = (*ItemPointer)->ItemS->HasCustomDisplayMesh();
		if (bWasPreviousItemEmpty)
		{
			if (bCustomMesh)
			{
				CustomMesh->SetVisibility(true);
				ItemMesh->SetVisibility(false);
			}
			else
			{
				ItemMesh->SetVisibility(true);
				CustomMesh->SetVisibility(false);
			}
		}

		if (bCustomMesh)
		{
			TArray<FVector> Verts;
			TArray<FVector2D> UVs;
			TArray<int32> Tris;
			TArray<FVector> Normals;
			UMaterial* Mat;
			(*ItemPointer)->ItemS->GetCustomDisplayMesh(CustomMesh, Verts, UVs, Tris, Normals, Mat);
			MeshProvider->SetupMaterialSlot(0, TEXT("Custom Mesh Material Slot"), Mat);
			MeshProvider->CreateSectionFromComponents(
				0,
				0,
				0,
				Verts,
				Tris,
				Normals,
				UVs,
				TArray<FColor>(),
				TArray<FRuntimeMeshTangent>(),
				ERuntimeMeshUpdateFrequency::Infrequent,
				false
			);
			// CustomMesh->CreateMeshSection(0, Verts, Tris, Normals, UVs, TArray<FColor>(), TArray<FRuntimeMeshTangent>(), false);
			// CustomMesh->SetMaterial(0, Mat);
		}
		else
		{
			ItemMaterial->SetTextureParameterValue(TEXT("Item Material"), (*ItemPointer)->ItemS->GetTexture());
			ItemMesh->SetMaterial(0, ItemMaterial);
		}
	}
	else
	{
		ItemMesh->SetVisibility(false);
		CustomMesh->SetVisibility(false);
		bWasPreviousItemEmpty = true;
	}
}

void UItemMeshComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunc)
{
	if ((*ItemPointer)->ItemS && PreviousItem.operator!=(*(*ItemPointer)->ItemS))
	{
		OnItemChange();
		PreviousItem = *(*ItemPointer)->ItemS;
	}
}

void UItemMeshComponent::BeginPlay()
{
	Super::BeginPlay();

	ItemMesh = NewObject<UStaticMeshComponent>(this, TEXT("Item Mesh"));
	CustomMesh = NewObject<URuntimeMeshComponent>(this, TEXT("Custom Mesh"));
	MeshProvider = NewObject<URuntimeMeshProviderStatic>(this);
	CustomMesh->Initialize(MeshProvider);
	ItemMesh->SetupAttachment(this);
	CustomMesh->SetupAttachment(this);
	ItemMesh->RegisterComponent();
	CustomMesh->RegisterComponent();
	ItemMesh->SetStaticMesh(TmpItemMesh);
	ItemMaterial = UMaterialInstanceDynamic::Create(TmpItemMaterial, this);
	ItemMesh->SetCollisionProfileName(TEXT("NoCollision"));
	CustomMesh->SetCollisionProfileName(TEXT("NoCollision"));
}

UItemMeshComponent::UItemMeshComponent() 
{
	PrimaryComponentTick.bCanEverTick = true;
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ItemMeshFinder(TEXT("StaticMesh'/Game/Meshes/Item/AutoItem_16px.AutoItem_16px'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> ItemMaterialFinder(TEXT("Material'/Game/Materials/Items/ItemMaterial.ItemMaterial'"));
	TmpItemMesh = ItemMeshFinder.Object;
	TmpItemMaterial = ItemMaterialFinder.Object;
}

void UItemMeshComponent::SetItem(FItemStack const ** NewItemStackPointer)
{
	ItemPointer = NewItemStackPointer;
	PreviousItem = *(*ItemPointer)->ItemS;
	OnItemChange();
}

void UItemMeshComponent::SetItemMeshOffset(const FTransform & Offset)
{
	ItemMesh->AddLocalTransform(Offset);
}

void UItemMeshComponent::SetCustomMeshOffset(const FTransform & Offset)
{
	CustomMesh->AddLocalTransform(Offset);
}
