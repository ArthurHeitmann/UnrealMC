


#include "B_Log.h"

B_Log::B_Log()
{
	BlockID = 17;
	BlockModelType = BLOCK;
	BreakTime = 3;
	BreakingAction = AWood;
}
TArray<FVector2D> B_Log::GetTopUVs()
{
	return {
		{0.5, 0},
		{1, 0},
		{1, 1},
		{0.5, 1}
	};
}

TArray<FVector2D> B_Log::GetFrontUVs()
{
	return {
		{0, 0},
		{0.5, 0},
		{0.5, 1},
		{0, 1}
	};
}

TArray<FVector2D> B_Log::GetRightUVs()
{
	return {
		{0, 0},
		{0.5, 0},
		{0.5, 1},
		{0, 1}
	};
}

TArray<FVector2D> B_Log::GetBottomUVs()
{
	return {
		{0.5, 1},
		{0.5, 0},
		{1, 0},
		{1, 1}
	};
}

TArray<FVector2D> B_Log::GetLeftUVs()
{
	return {
		{0.5, 0},
		{0.5, 1},
		{0, 1},
		{0, 0}
	};
}

TArray<FVector2D> B_Log::GetBackUVs()
{
	return {
		{0, 0},
		{0.5, 0},
		{0.5, 1},
		{0, 1}
	};
}

TArray<FVector2D> B_Log::GetAllUVs()
{
	return {
		{0.5, 0},
		{1, 0},
		{1, 1},
		{0.5, 1},
		{0, 0},
		{0.5, 0},
		{0.5, 1},
		{0, 1},
		{0.5, 1},
		{0.5, 0},
		{1, 0},
		{1, 1},
		{0.5, 0},
		{0.5, 1},
		{0, 1},
		{0, 0},
		{0, 0},
		{0.5, 0},
		{0.5, 1},
		{0, 1},
		{0, 0},
		{0.5, 0},
		{0.5, 1},
		{0, 1}
	};
}