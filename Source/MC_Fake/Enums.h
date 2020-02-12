#pragma once

enum EDirection {
	TOP, EAST, BOTTOM, WEST, SOUTH, NORTH
};

enum EBlockModelType {
	NONE, BLOCK, WATER, ITEM
};

enum EItemActions {
	ANoAction, AEat, AWood, APickaxe, ABreakSand, ABreakPlant, AAttack
};

enum EPostItemUseTask {
	NoTask, 
	Decrement, //Remove N Items from Stack
	RemoveCharges, //Remove N charges from Item
	Destroy	//Destroy Item
};