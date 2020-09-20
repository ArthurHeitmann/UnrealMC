


#include "FileIO.h"
#include "HAL/PlatformFile.h"
#include "HAL/PlatformFilemanager.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "Misc/Paths.h"
#include "../Blocks/B_Block.h"
#include "../World/Chunk.h"
#include "Enums.h"
#include "Serialization/Archive.h"
#include "Serialization/BufferArchive.h"
#include "../Blocks/B_Air.h"


ChunkData FileIO::LoadChunk(const FString& WorldName, int PosX, int PosY)
{/*
	FString ChunkPath(FPaths::ProjectSavedDir());
	ChunkPath += "Worlds/";
	ChunkPath += WorldName;
	ChunkPath += "/Chunk_X_";
	ChunkPath += FString::FromInt(PosX);
	ChunkPath += "_Y_";
	ChunkPath += FString::FromInt(PosY);

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!PlatformFile.FileExists(*ChunkPath))
		return ChunkData();
	IFileHandle* FileHandle = PlatformFile.OpenRead(*ChunkPath);
	if (!FileHandle)
		return ChunkData();*/

	ChunkData OutData;
	/*uint8* Bytes = (uint8*) FMemory::Malloc(PlatformFile.FileSize(*ChunkPath));
	FileHandle->Read(Bytes, PlatformFile.FileSize(*ChunkPath));
	OutData.LastTimeUpdated = *reinterpret_cast<uint64*>(Bytes);
	OutData.GeneratorVersion = *reinterpret_cast<uint8*>(Bytes + 8);
	OutData.NextGenerationStage = *reinterpret_cast<uint8*>(Bytes + 9);

	int64 currentByte = 10;
	int x = 0, y = 0, z = 0;
	while (Bytes[currentByte] != 255)
	{
		OutData.BlockReference[x][y][z] = Block::GetBlockFromBytes(Bytes + currentByte);
		if (OutData.BlockReference[x][y][z] != B_Air::AirRef)
		{
			if ((Bytes[currentByte + 2] & 128) == 0)
				currentByte += 3;
			else
			{
				uint64 PropertyLength = *reinterpret_cast<uint64*>(Bytes[currentByte + 3]);
				currentByte += 3 + 8 + PropertyLength;
			}
		}
		else
			currentByte += 2;


		y++;
		if (y == 16)
		{
			y = 0;
			x++;
			if (x == 16)
			{
				x = 0;
				z++;
				if (z == 256)
					break;
			}
		}
	}

	delete FileHandle;
	FMemory::Free(Bytes);*/

	return OutData;
}

void FileIO::SaveChunk(UChunk* Chunk, const FString& Worldname)
{
	/*IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	FString WorldPath(FPaths::ProjectSavedDir());
	WorldPath += "Worlds/";
	WorldPath += Worldname;
	WorldPath += "/";
	
	if (!PlatformFile.DirectoryExists(*WorldPath))
	{
		PlatformFile.CreateDirectoryTree(*WorldPath);
		if (!PlatformFile.DirectoryExists(*WorldPath))
		{
			UE_LOG(LogTemp, Error, TEXT("! Couldn't Create Saving Directory !"));
			return;
		}
	}
	FString FilePath(WorldPath);
	FilePath += "Chunk_X_";
	FilePath += FString::FromInt(Chunk->GetActorLocation().X / 1600);
	FilePath += "_Y_";
	FilePath += FString::FromInt(Chunk->GetActorLocation().Y / 1600);
	
	TArray<uint8> Bytes = ChunkToBytes(Chunk);
	
	IFileHandle* FileHandle = PlatformFile.OpenWrite(*FilePath);
	if (FileHandle)
	{
		FileHandle->Write(Bytes.GetData(), Bytes.Num());
		delete FileHandle;
	}*/
}

TArray<uint8> FileIO::ChunkToBytes(UChunk* Chunk)
{
	TArray<uint8> Bytes;
	//Bytes.Reserve(20000);
	////int64 LastTimeUpdated = Chunk->GetLastTimeUpdated();
	//int64 LastTimeUpdated = Chunk->GetLastTimeUpdated();
	//uint8* TimeBytes = (uint8*) FMemory::Malloc(sizeof(int64));
	//TimeBytes = reinterpret_cast<uint8*>(&LastTimeUpdated);
	//for (int byte = 0; byte < 8; byte++)
	//	Bytes.Add(TimeBytes[byte]);
	//Bytes.Add(0);	//Gen Version
	//Bytes.Add((uint8) Chunk->GetNextGenerationStage());

	//TArray<TArray<TArray<Block*>>>* ChunkBlockData = Chunk->GetChunkBlockData();
	//int LastAirIter = -1;

	//for (int z = 0; z < 256; z++)
	//{
	//	for (int x = 0; x < 16; x++)
	//	{
	//		for (int y = 0; y < 16; y++)
	//		{
	//			if ((*ChunkBlockData)[x][y][z]->GetBlockEnum() == BAir && LastAirIter < 0)
	//				LastAirIter = Bytes.Num();
	//			else if ((*ChunkBlockData)[x][y][z]->GetBlockEnum() != BAir && LastAirIter != -1)
	//				LastAirIter = -1;
	//			Bytes.Append((*ChunkBlockData)[x][y][z]->GetBinaryData());
	//		}
	//	}
	//}
	//Bytes[LastAirIter] = 255;
	//Bytes.SetNum(LastAirIter + 1, true);

	return Bytes;
}

bool FileIO::DoesChunkExist(const FString& WorldName, int PosX, int PosY)
{
	/*FString WorldPath(FPaths::ProjectSavedDir());
	WorldPath += "Worlds/";
	WorldPath += WorldName;
	WorldPath += "/";
	FString FilePath(WorldPath);
	FilePath += "Chunk_X_";
	FilePath += FString::FromInt(PosX);
	FilePath += "_Y_";
	FilePath += FString::FromInt(PosY);

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	return PlatformFile.FileExists(*FilePath);*/
	return false;
}
