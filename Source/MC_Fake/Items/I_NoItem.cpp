


#include "I_NoItem.h"
#include "Enums.h"

I_NoItem::I_NoItem()
{
	ItemName = TEXT("No Item");
	Id = 0;
}

bool I_NoItem::IsStackableWith(I_Item* OtherItem)
{
	return false;
}
