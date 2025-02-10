// ItemStructure.cpp
#include "SurvivalGame/Public/Data/Struct/ItemStructure.h"

FItemStructure::FItemStructure()
    : RegistryKey(NAME_None)
    , ItemQuantity(1)
    , StackSize(1)
    , CurrentHP(100.0f)
    , MaxHP(100.0f)
    , CurrentAmmo(0)
    , MaxAmmo(0)
{
}