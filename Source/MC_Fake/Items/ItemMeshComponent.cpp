


#include "ItemMeshComponent.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/StaticMeshComponent.h"
#include "Item.h"

void UItemMeshComponent::OnItemChange()
{
	if ((*ItemPointer)->ItemCount > 0)
	{
		bool bCustomMesh = (*ItemPointer)->ItemS->HasCustomDisplayMesh();
		if (bWasPreviouseItemEmpty)
		{
			if (bCustomMesh)
				CustomMesh->SetVisibility(true);
			else
				ItemMesh->SetVisibility(true);
		}

		if (bCustomMesh)
		{
			TArray<FVector> Verts;
			TArray<FVector2D> UVs;
			TArray<int32> Tris;
			TArray<FVector> Normals;
			UMaterial* Mat;
			(*ItemPointer)->ItemS->GetCustomDisplayMesh(CustomMesh, Verts, UVs, Tris, Normals, Mat);
			CustomMesh->CreateMeshSection(0, Verts, Tris, Normals, UVs, TArray<FColor>(), TArray<FRuntimeMeshTangent>(), false);
			CustomMesh->SetMaterial(0, Mat);
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
		bWasPreviouseItemEmpty = true;
	}
}

void UItemMeshComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunc)
{
	if ((*ItemPointer)->ItemS && PreviousItem.operator!=(*(*ItemPointer)->ItemS))
	{
		OnItemChange();
		PreviousItem = *(*ItemPointer)->ItemS;
		auto l = GetComponentLocation();
		UE_LOG(LogTemp, Warning, TEXT("x %f y %f z %f"), l.X, l.Y, l.Z);
	}
}

void UItemMeshComponent::BeginPlay()
{
	Super::BeginPlay();

	ItemMesh = NewObject<UStaticMeshComponent>(this, TEXT("Item Mesh"));
	CustomMesh = NewObject<URuntimeMeshComponent>(this, TEXT("Custom Mesh"));
	ItemMesh->SetupAttachment(this);
	CustomMesh->SetupAttachment(this);
	ItemMesh->RegisterComponent();
	CustomMesh->RegisterComponent();
	ItemMesh->SetRelativeTransform(GetComponentTransform());
	CustomMesh->SetRelativeTransform(GetComponentTransform());
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
	CustomMesh->AddLocalTransform(Offset);
}

void UItemMeshComponent::SetCustomMeshOffset(const FTransform & Offset)
{
	CustomMesh->AddLocalTransform(Offset);
}
