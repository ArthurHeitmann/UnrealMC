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
		return dynamic_cast<B_Block*>(BlockMakerMap[BlockName]());
	}

	static inline B_Block* GetStaticBlock(FName BlockName)
	{
		return dynamic_cast<B_Block*>(StaticBlocksMap[BlockName]);
	}

	static void InitializeAll();
};


#define BlockRegistryDefinition(className) class className##Helper \
{\
	static className##Helper className##Initializer;\
public:\
	className##Helper() \
	{\
		BlockManager::RegisterBlock<className>(FName(#className));\
	}\
};


#define BlockRegistryImplementation(className) className##Helper className##Helper::className##Initializer = className##Helper();


#define BlockStaticRegistryImplementation(className, BlockVal) class className##Helper \
{\
	static className##Helper className##Initializer;\
public:\
	className##Helper() \
	{\
		BlockManager::RegisterStaticBlock(FName(#className), BlockVal);\
	}\
}; \
\
className##Helper className##Init;
