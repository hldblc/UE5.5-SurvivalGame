// ItemStructure.cpp

#include "SurvivalGame/Public/Data/Struct/ItemStructure.h"

FItemStructure::FItemStructure()
    : RegistryKey(NAME_None)  // Initialize RegistryKey to None
    , ItemType(E_ItemType::None)
    , ItemCategory(E_ItemCategory::None)
    , ItemQuantity(1)
{
}

void FItemStructure::InitializeFromData(const FItemStructure& LoadedItem)
{
    // Copy basic properties
    ItemName = LoadedItem.ItemName;
    ItemType = LoadedItem.ItemType;
    ItemCategory = LoadedItem.ItemCategory;
    ItemQuantity = LoadedItem.ItemQuantity;
}

bool FItemStructure::operator==(const FItemStructure& Other) const
{
    // Simple equality check for basic identification
    return RegistryKey == Other.RegistryKey;
}