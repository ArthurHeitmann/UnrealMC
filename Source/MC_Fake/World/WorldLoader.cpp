//#include "WorldLoader.h"
//#include "Engine/World.h"
//#include "Chunk.h"
//
//void UWorldLoader::CheckOrSpawnChunk(int32 x, int32 y)
//{
//	int64 Key = x << 16 | y;
//	if (!LoadedChunks.Find(Key) && World)
//		LoadedChunks.Add(Key, World->SpawnActor<Chunk>(FVector(x, y, 0), FRotator::ZeroRotator));
//}
//
//void UWorldLoader::SetWorld(UWorld* NewWorld)
//{
//	if (NewWorld)
//		World = NewWorld;
//}
