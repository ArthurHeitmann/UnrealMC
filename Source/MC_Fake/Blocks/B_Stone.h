// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Block.h"

/**
 * 
 */
class MC_FAKE_API B_Stone : public Block
{
	
protected:
	//static UTexture* StoneTexture;

public:
	B_Stone();

	virtual Block* Clone() override;
};