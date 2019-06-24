#pragma once

enum EAllBlocks {
	BAir, BStone, BDirt, BGrass, BWater, BLog_Oak, BLeaves_Oak
};

enum EAllItems {
	INoItem, IStone, IDirt, IGrass, ILog_Oak, ILeaves_Oak, IPickaxe_Stone
};

enum EFaceDirection {
	TOP, RIGHT, BOTTOM, LEFT, SOUTH, NORTH
};

enum EBlockModelType {
	NONE, BLOCK, WATER, ITEM
};

enum EItemActions {
	ANoAction, AEat, AWood, APickaxe, ABreakSand, ABreakPlant, AAttack
};

enum EPostItemUseTask {
	NoTask, Decrement, RemoveCharges, Destroy, Remove
};