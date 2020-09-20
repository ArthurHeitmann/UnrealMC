


#include "I_Pickaxe_Stone.h"
#include "UObject/ConstructorHelpers.h"

I_Pickaxe_Stone::I_Pickaxe_Stone()
{
	Id = 274;
	ItemName = TEXT("Pickaxe Stone");
	Durability = Charges = 132;
	BreakingSpeed = 6.5;

	static ConstructorHelpers::FObjectFinder<UTexture> TextureFinder(TEXT("Texture2D'/Game/Materials/Textures/Items/stone_pickaxe.stone_pickaxe'"));
	Texture = TextureFinder.Object;
}
