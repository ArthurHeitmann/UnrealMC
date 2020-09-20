

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class MC_FAKE_API McStaticFunctions
{
public:

    static FORCEINLINE int32 SmartMod(int32 Num, int32 Div)
    {
        int32 Mod = Num % Div;
        return Mod >= 0 ? Mod : Mod + Div;
    }
};
