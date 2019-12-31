


#include "ChunkSaveGame.h"

void UChunkSaveGame::ParseData(const TArray<TArray<TArray<B_Block*>>>& NewData)
{
	ChunkBlockData.SetNum(16);
	for (int x = 0; x < 16; x++)
	{
		ChunkBlockData[x].SetNum(16);
		for (int y = 0; y < 16; y++)
		{
			ChunkBlockData[x][y].Reserve(256);
			for (int z = 0; z < 256; z++)
			{
				ChunkBlockData[x][y].Add(*NewData[x][y][z]->Clone());
			}
		}
	}

}

TArray<TArray<TArray<B_Block*>>> UChunkSaveGame::GetData()
{
	TArray<TArray<TArray<B_Block*>>> out;

	out.SetNum(16);
	for (int x = 0; x < 16; x++)
	{
		out[x].SetNum(16);
		for (int y = 0; y < 16; y++)
		{
			out[x][y].SetNumUninitialized(256);
			for (int z = 0; z < 256; z++)
			{
				out[x][y][z] = ChunkBlockData[x][y][z].Clone();
			}
		}
	}

	return out;
}
