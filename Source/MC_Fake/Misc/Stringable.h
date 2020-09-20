#pragma once
#include "UnrealString.h"

class IStringable
{
public:
    virtual FString toString() = 0;
protected:
    virtual ~IStringable() {};
};
