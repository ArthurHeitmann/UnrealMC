#include "ItemRenderer.h"
#include "Components/SceneCaptureComponent2D.h"
#include "../Items/ItemMeshComponent.h"
#include "../Items/ItemStack.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "ConstructorHelpers.h"
#include "Kismet/KismetRenderingLibrary.h"

AItemRenderer::AItemRenderer()
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Scene Capture"));
	ItemMesh = CreateDefaultSubobject<UItemMeshComponent>(TEXT("Item Mesh"));
	UWorld* World = AActor::GetWorld();
	if (World)
	{
		RenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(World, 150, 150);
		SceneCapture->TextureTarget = RenderTarget;
	}
	SceneCapture->CaptureSource = ESceneCaptureSource::SCS_SceneColorHDR;
	SceneCapture->ProjectionType = ECameraProjectionMode::Type::Orthographic;
	SceneCapture->OrthoWidth = 90;
	SceneCapture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	SceneCapture->ShowOnlyActors.Add(this);
	SceneCapture->SetRelativeRotation(FRotator(-15.f, 0, 0));
	SceneCapture->SetupAttachment(GetRootComponent());
	ItemMesh->SetupAttachment(GetRootComponent());
	

	ConstructorHelpers::FObjectFinder<UMaterial> MatFinder(TEXT("Material'/Game/Materials/Items/M_ItemPreview.M_ItemPreview'"));
	UIMaterialLoaded = MatFinder.Object;

}

void AItemRenderer::InitWithItemStack(const FItemStack& ItemStack)
{
	if (!PPIS)
		PPIS = new const FItemStack*;
	*PPIS = &ItemStack;
	ItemMesh->SetItem(PPIS);
}

UMaterialInstanceDynamic* AItemRenderer::GetUIMaterial(UObject* MatOwner)
{
	if (!UIMaterial)
	{
		UIMaterial = UMaterialInstanceDynamic::Create(UIMaterialLoaded, MatOwner);
		UIMaterial->SetTextureParameterValue(TEXT("Render Target Texture"), RenderTarget);
	}
	return UIMaterial;
}

void AItemRenderer::BeginPlay()
{
	Super::BeginPlay();

	FVector TotalOffset(0, 0, -1000);

	FTransform Offset(FVector(80.f, -35.f, -50.f));
	Offset.SetScale3D({ 0.5, 0.5, 0.5 });
	Offset.SetRotation(FQuat(FRotator(0.f, 45.f, 0.f)));
	ItemMesh->SetItemMeshOffset(Offset);
	ItemMesh->SetCustomMeshOffset(Offset);

	AddActorWorldOffset(TotalOffset);
}

void AItemRenderer::EndPlay(EEndPlayReason::Type Reason)
{
	if (PPIS)
		delete PPIS;
}

void AItemRenderer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

