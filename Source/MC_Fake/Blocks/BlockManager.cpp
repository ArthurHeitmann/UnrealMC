#pragma message("McClonable .cpp 1")
#include "BlockManager.h"
#pragma message("McClonable .cpp 2")


TMap<FName, McCloneable* (*)()> BlockManager::BlockMakerMap;
TMap<FName, McCloneable*> BlockManager::StaticBlocksMap;