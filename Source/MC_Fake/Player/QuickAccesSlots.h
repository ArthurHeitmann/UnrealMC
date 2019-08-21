
#include "CoreMinimal.h"
#include "ItemSlots.h"
#include "QuickAccesSlots.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MC_FAKE_API UQuickAccesSlots : public UItemSlots
{
	GENERATED_BODY()

private:
	int32 SelectedItemId;
	//TODO remove below
	//class Item** SelectedItem;
	
public:
	UQuickAccesSlots();

	//void SetSelecteddItemPointer(class Item** NewItemPointer);

	FItemStack& SetSelecteddItemSlotById(int32 id);

	FItemStack& NextSelectedItem();

	FItemStack& PreviousSelectedItem();

};