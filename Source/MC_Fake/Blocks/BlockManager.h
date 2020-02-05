#pragma once

#include "CoreMinimal.h"
#include "B_Block.h"
#pragma message("McClonable .h")
#include "McClonable.h"

class BlockManager
{
private:
	static TMap<FName, McCloneable * (*)()> BlockMakerMap;
	static TMap<FName, McCloneable*> StaticBlocksMap;

	template<class T>
	static McCloneable* MakeBlockInternal()
	{
		return new T;
	}
	
public:
	template<class T>
	static void RegisterBlock(FName BlockName)
	{
		BlockMakerMap.Add(BlockName, &MakeBlockInternal<T>);
	}

	static void RegisterStaticBlock(FName BlockName, McCloneable* B)
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

	static void InitializeAll()
	{
		for (auto funcPair : BlockMakerMap)
		{
			//create an instance and delete it right away
			//Basically just calling the constructor with extra steps
			delete funcPair.Value();
		}
	}
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
