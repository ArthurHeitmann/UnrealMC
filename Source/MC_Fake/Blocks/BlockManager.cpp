#include "BlockManager.h"
#include "Blocks/B_Air.h"
#include "Blocks/B_Dirt.h"
#include "Blocks/B_Grass.h"
#include "Blocks/B_LeavesOak.h"
#include "Blocks/B_LogOak.h"
#include "Blocks/B_Stone.h"
#include "Blocks/B_Water.h"


TMap<FName, B_Block* (*)()> BlockManager::BlockMakerMap;
TMap<FName, B_Block*> BlockManager::StaticBlocksMap;

void BlockManager::InitializeAll()
{
	RegisterStaticBlock("Air", new B_Air);
	RegisterBlock<B_Dirt>("Dirt");
	RegisterBlock<B_Grass>("Grass");
	RegisterBlock<B_LeavesOak>("LeavesOak");
	RegisterBlock<B_LogOak>("LogOak");
	RegisterBlock<B_Water>("Water");
	RegisterBlock<B_Stone>("Stone");

	for (auto& blockGen : BlockMakerMap)
		delete blockGen.Value();
}
