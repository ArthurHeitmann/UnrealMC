

#pragma once

#include "CoreMinimal.h"
#include "Enums.h"
#include "Engine/EngineTypes.h"

/**
 * Holds all Data of an Item.
 */
class MC_FAKE_API I_Item
{

public:
	/** This struct says what should be done with an Item (-stack) after it has been used */
	struct PostUseTask {
		EPostItemUseTask Tasks;
		int32 Count;
	};

protected:
	/** Item name visible to the User */
	FName ItemName;
	/** Unique ID for this Item type */
	uint16 Id;
	/** Set of things this Item can be used for (attacking, mining, eating, etc.) */
	TSet<EItemActions> ItemActions;
	/** The Damage this Item deals when hitting an Entity */
	float AttackDamage;
	/**
	 * A speed factor that is applied when mining a block.
	 * Only applicable if this Item is specialized for this Block type.
	 */
	float BreakingSpeed;
	/** How many charges this Item has. Only used for tools */
	int32 Strength;
	/** Whether this Item can be stacked with another one of the same type */
	bool bIsStackable;
	/** How large the stack can grow. Default = 64 */
	int32 MaxStackCount;
	/** The Texture that gets applied to the default mesh. */
	class UTexture* Texture;
	/** True: This Item doesn't use the default Mesh and has a custom one.  */
	bool bCustomDisplayMesh;

public:
	/** Initializes this Item with default properties */
	I_Item();

	FName GetName();
	uint16 GetId();
	TSet<EItemActions> GetItemActions();
	float GetAttackDamage();
	float GetBreakingSpeed();
	int GetStrength();
	bool IsItemStackable();
	int32 GetMaxStackCount();
	virtual bool IsStackableWith(I_Item* OtherItem);
	bool HasCustomDisplayMesh();
	virtual void GetCustomDisplayMesh(UObject* Base, TArray<FVector>& VerticesOut, TArray<FVector2D>& UVsOut, 
		TArray<int32>& TrianglesOut, TArray<FVector>& NormalsOut, class UMaterial*& MatOut);

	virtual bool operator==(const I_Item& I);
	virtual bool operator!=(const I_Item& I);

	/** This function gets called whenever the user interacts with this Item */
	virtual PostUseTask OnItemUse(const FHitResult& HitPointData, class AMcWorld* World);

	class UTexture* GetTexture();

	virtual ~I_Item();
};
