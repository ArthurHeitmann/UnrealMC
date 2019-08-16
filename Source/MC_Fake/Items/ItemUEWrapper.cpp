


#include "ItemUEWrapper.h"
#include "Item.h"

FName UItemUEWrapper::GetItemName()
{
	if (!ItemRef)
		return "";
	return ItemRef->GetName();
}

int32 UItemUEWrapper::GetItemId()
{
	if (!ItemRef)
		return 0;
	return ItemRef->GetId();
}

void UItemUEWrapper::SetItem(Item* NewItem)
{
	ItemRef = NewItem;
}
