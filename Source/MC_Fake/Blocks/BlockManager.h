#pragma once

#include "CoreMinimal.h"
#include "B_Block.h"

class BlockManager
{
private:
	static TMap<FName, B_Block * (*)()> BlockMakerMap;
	static TMap<FName, B_Block*> StaticBlocksMap;

	template<class T>
	static B_Block* MakeBlockInternal()
	{
		return new T;
	}
	
public:
	template<class T>
	static void RegisterBlock(FName BlockName)
	{
		BlockMakerMap.Add(BlockName, &MakeBlockInternal<T>);
	}

	static void RegisterStaticBlock(FName BlockName, B_Block* B)
	{
		StaticBlocksMap.Add(BlockName, B);
	}

	static inline B_Block* GetBlock(FName BlockName)
	{
		return BlockMakerMap[BlockName]();
	}

	static inline B_Block* GetStaticBlock(FName BlockName)
	{
		return StaticBlocksMap[BlockName];
	}

};


#define BlockRegistryImplementation(className) class className##Helper \
{\
	className Initializer; \
public:\
	className##Helper() \
	{\
		BlockManager::RegisterBlock<className>(FName(#className));\
	}\
}; \
\
className##Helper className##Init;

#define BlockStaticRegistryImplementation(className, BlockVal) class className##Helper \
{\
	className Initializer; \
public:\
	className##Helper() \
	{\
		BlockManager::RegisterStaticBlock(FName(#className), BlockVal);\
	}\
}; \
\
className##Helper className##Init;
