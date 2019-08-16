
#include "CoreMinimal.h"
#include "ItemSlots.h"
#include "QuickAccesSlots.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MC_FAKE_API UQuickAccesSlots : public UItemSlots
{
	GENERATED_BODY()

private:
	int32 SelectedItemId;
	class Item** SelectedItem;
	
public:
	UQuickAccesSlots();

	void SetSelecteddItemPointer(class Item** NewItemPointer);

	void SetSelecteddItemSlotById(int32 id);

	void NextSelectedItem();

	void PreviousSelectedItem();

};