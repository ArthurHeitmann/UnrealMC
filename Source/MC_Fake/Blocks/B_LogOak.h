

#pragma once

#include "CoreMinimal.h"
#include "B_Log.h"
#include "BlockManager.h"

/**
 * 
 */
class MC_FAKE_API B_LogOak : public B_Log
{
public:
	B_LogOak();


	virtual B_Block* Clone() override;
};

