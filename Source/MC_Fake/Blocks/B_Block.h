#pragma once

#include "CoreMinimal.h"
#include "../World/McWorld.h"
#include "Enums.h"


/**
 * This class holds all data of a Block/Voxel.
 *
 * Since there will be millions of blocks in the world, the size should be kept small (fewer attributes or make them static)
 */
class MC_FAKE_API B_Block
{
	/**
	 * A helper struct ofr storing possible item drops
	 */
	struct ItemDrops { float Chance; class Item* Item; };

protected:
	/**
	 * The base Material from which on a UMaterialInstanceDynamic will be created.
	 * The Block specific texture will then be applied to the instantiated material.
	 */
	static class UMaterial* BlockMaterial;
	/** Every Block type has a unique ID */
	uint16 BlockID;
	/** If a Block has subtypes they can use this SubID */
	uint8 SubID;
	/** This name will be shown to the user */
	FName BlockName;
	/** The block model is important when generating the chunk mesh and optimizing faces away */
	EBlockModelType BlockModelType;
	/** If an Item has this Action property it can break the block faster. */
	EItemActions BreakingAction;
	/**
	 * Each block as it's own enum.
	 * This should be replaced in the future with the BlockName, since this makes it more complicated to add new classes
	 */
	EAllBlocks BlockEnum;
	/** This texture get's applied to the generic Block material*/
	class UTexture* Texture;
	/** This bump map texture get's applied to the generic Block material*/
	class UTexture* TextureBMP;
	/** The time it takes to brakes this Block when not using any special Items */
	float BreakTime;
	/**
	 * All possible weighted Item drops.
	 * They get chosen randomly when breaking a block.
	 * Only used when bCustomItemDrops is True
	 */
	TArray<ItemDrops> PossibleItemDrops;
	/** If False this Block will drop itself; If True it will go through the PossibleItemDrops */
	bool bCustomItemDrops;
	/** Whether this block has a custom Mesh for handling collision (with the player and the Line Traces) */
	bool bCustomCollisionMesh;

	/**
	 *Drops an Item where the Block has been broken
	 *
	 * @param World World Context Object
	 * @param Location The Location in World Space to drop the Item (usually the Blocks position)
	 * @param DropItem
	 */
	void DropItem(class UWorld* World, FVector Location, class Item* DropItem);

public:
	/** Initialized the Block with some default values */
	B_Block();
	virtual ~B_Block();

	static B_Block* GetBlockFromBytes(uint8* Bytes);

	/**
	 * @return If a side is optimizable it means that it can be safely removed from a mesh if
	 * that face is completely hidden by something else.
	 *
	 * @param Direction The side of the block that contains these faces
	 */
	virtual bool IsSideOptimizable(EDirection Direction);
	/** @return True if the Block is solid; False if the block is transparent */
	virtual bool IsBlockOpaque();
	/** If True no Block sides will be optimized. All Vertices, Triangles, etc. will be retrieved in one call. */
	virtual bool UsesCustomModel();
	virtual FName GetName();
	virtual EBlockModelType GetBlockModelType();
	virtual EAllBlocks GetBlockEnum();
	virtual EItemActions GetBreakingAction();
	/** Creates a Material Instance from the default Material and applies the block specific textures to it. */
	virtual class UMaterialInstanceDynamic* GetMaterial(UObject* UObj);
	virtual UTexture* GetTexture();
	virtual UTexture* GetTextureBMP();
	float GetBreakTime();
	virtual B_Block* Clone();
	virtual TArray<uint8> GetBinaryData();
	virtual bool operator==(const B_Block& B);
	virtual bool operator!=(const B_Block& B);

	/**
	 * This function is called when the Block get's placed in the world
	 *
	 * @param World World context object
	 * @param Location The Location in World Space where the Block is being broken
	 */
	virtual void OnPlace(class UWorld* World, FVector Location);
	/**
	 * This function get's called when the player interacts with the block 
	 *
	 * @param World World context object
	 * @param Location The Location in World Space where the Block is being broken
	 */
	virtual void OnInteract(class UWorld* World, FVector Location);
	/**
	 * This function get's called when someone starts breaking the block 
	 *
	 * @param World World context object
	 * @param Location The Location in World Space where the Block is being broken
	 */
	virtual void OnBeginBreak(class UWorld* World, FVector Location);
	/**
	 * This function get's called every tick when someone is breaking the block 
	 *
	 * @param World World context object
	 * @param Location The Location in World Space where the Block is being broken
	 * @param Seconds The time the player has spent hitting the block.
	 */
	virtual void UpdateBreak(class UWorld* World, FVector Location, float Seconds);
	/**
	 * This function get's called when someone breaks the block 
	 *
	 * @param World World context object
	 * @param Location The Location in World Space where the Block is being broken
	 */
	virtual void OnBreak(class UWorld* World, FVector Location);

	/**
	 * @return All coordinates of the vertices of the top face
	 *
	 * @param x, y, z All coordinates will be offset from this point
	 */
	virtual TArray<FVector> GetTopVertices(float x, float y, float z);
	/**
	 * @return All coordinates of the vertices of the right face
	 *
	 * @param x, y, z All coordinates will be offset from this point
	 */
	virtual TArray<FVector> GetRightVertices(float x, float y, float z);
	/**
	 * @return All coordinates of the vertices of the bottom face
	 *
	 * @param x, y, z All coordinates will be offset from this point
	 */
	virtual TArray<FVector> GetBottomVertices(float x, float y, float z);
	/**
	 * @return All coordinates of the vertices of the left face
	 *
	 * @param x, y, z All coordinates will be offset from this point
	 */
	virtual TArray<FVector> GetLeftVertices(float x, float y, float z);
	/**
	 * @return All coordinates of the vertices of the front face
	 *
	 * @param x, y, z All coordinates will be offset from this point
	 */
	virtual TArray<FVector> GetFrontVertices(float x, float y, float z);
	/**
	 * @return All coordinates of the vertices of the back face
	 *
	 * @param x, y, z All coordinates will be offset from this point
	 */
	virtual TArray<FVector> GetBackVertices(float x, float y, float z);
	virtual TArray<FVector> GetAllVertices(float x, float y, float z);

	//The following function return an array of indices for the triangle.
	//3 consecutive indices form a triangle.
	//The vertices and indices should be counter clockwise (?) so that the default normals work properly.
	virtual TArray<int32> GetTopTrianglesFrom(int32 Start);
	virtual TArray<int32> GetRightTrianglesFrom(int32 Start);
	virtual TArray<int32> GetBottomTrianglesFrom(int32 Start);
	virtual TArray<int32> GetLeftTrianglesFrom(int32 Start);
	virtual TArray<int32> GetFrontTrianglesFrom(int32 Start);
	virtual TArray<int32> GetBackTrianglesFrom(int32 Start);
	virtual TArray<int32> GetAllTrianglesFrom(int32 Start = 0);

	//UV coordinates for the vertices of a blocks side
	virtual TArray<FVector2D> GetTopUVs();
	virtual TArray<FVector2D> GetRightUVs();
	virtual TArray<FVector2D> GetBottomUVs();
	virtual TArray<FVector2D> GetLeftUVs();
	virtual TArray<FVector2D> GetFrontUVs();
	virtual TArray<FVector2D> GetBackUVs();
	virtual TArray<FVector2D> GetAllUVs();

	//Normal vector of each vertex
	virtual TArray<FVector> GetTopNormals();
	virtual TArray<FVector> GetRightNormals();
	virtual TArray<FVector> GetBottomNormals();
	virtual TArray<FVector> GetLeftNormals();
	virtual TArray<FVector> GetFrontNormals();
	virtual TArray<FVector> GetBackNormals();
	virtual TArray<FVector> GetAllNormals();

};
