

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class MC_FAKE_API McStaticFunctions
{
public:

    static FORCEINLINE int32 SmartMod(int32 num, int32 div)
    {
        int32 mod = num % div;
        return mod >= 0 ? mod : mod + div;
    }
};
