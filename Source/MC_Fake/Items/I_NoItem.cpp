


#include "I_NoItem.h"
#include "Enums.h"

I_NoItem::I_NoItem()
{
	ItemName = TEXT("");
	ItemEnum = INoItem;
	Id = 0;
}

bool I_NoItem::IsStackableWith(Item* OtherItem)
{
	return false;
}
