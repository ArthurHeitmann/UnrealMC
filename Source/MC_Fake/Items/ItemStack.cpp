#include "ItemStack.h"

UItemUEWrapper* UItemStackHelper::ItemToItemWrapper(FItemStack I)
{
	UItemUEWrapper* iw = NewObject<UItemUEWrapper>();
	iw->SetItem(I.ItemS);
	return iw;
}
