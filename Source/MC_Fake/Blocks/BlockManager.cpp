#include "BlockManager.h"


TMap<FName, B_Block * (*)()> BlockManager::BlockMakerMap;
TMap<FName, B_Block*> BlockManager::StaticBlocksMap;