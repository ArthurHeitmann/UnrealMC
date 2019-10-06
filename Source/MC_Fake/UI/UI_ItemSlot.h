

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Items/ItemStack.h"
#include "UI_ItemSlot.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class MC_FAKE_API UUI_ItemSlot : public UUserWidget
{
	GENERATED_BODY()
	
private:
	class UTextBlock* NameText;
	class UTextBlock* NumberText;
	UPROPERTY(EditAnywhere)
	class UImage* RenderedImage;
	UPROPERTY(EditAnywhere)
	class AItemRenderer* ItemRenderer;
	UPROPERTY(EditAnywhere)
		class UMaterialInstanceDynamic* TestMat;

protected:
	virtual bool Initialize() override;
	virtual void NativeConstruct() override;

public:

	UPROPERTY(BlueprintReadOnly)
	FItemStack ItemStackC;

	UFUNCTION(BlueprintCallable)
	void SetItemStack(const FItemStack& NewItemStack);

	virtual ~UUI_ItemSlot() override;
};
