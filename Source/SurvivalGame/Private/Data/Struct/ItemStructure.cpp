// ItemStructure.cpp

#include "SurvivalGame/Public/Data/Struct/ItemStructure.h"

FItemStructure::FItemStructure()
    : RegistryKey(NAME_None)
    , ItemType(E_ItemType::None)
    , ItemCategory(E_ItemCategory::None)
    , ItemRarity(E_ItemRarity::Common)
    , bIsStackable(false)
    , MaxStackSize(1)
    , ItemQuantity(1)
    , bIsEquippable(false)
    , ToolType(E_ToolType::None)
    , WeaponType(E_WeaponType::None)
    , ArmorType(E_ArmorType::None)
    , bHasDurability(false)
    , CurrentDurability(100)
    , MaxDurability(100)
    , DurabilityDecayRate(0.0f)
    , bIsDestroyable(true)
    , WeightClass(E_WeightClass::Light)
    , bIsQuestItem(false)
    , bIsUnique(false)
    , InitialItemState(E_ItemState::Normal)
    , ItemState(E_ItemState::Normal)
{
    DefaultModifiers.Empty();
    ItemModifiers.Empty();
}

void FItemStructure::InitializeFromData(const FItemStructure& LoadedItem)
{
    ItemName = LoadedItem.ItemName;
    ItemType = LoadedItem.ItemType;
    ItemCategory = LoadedItem.ItemCategory;
    ItemRarity = LoadedItem.ItemRarity;
    bIsStackable = LoadedItem.bIsStackable;
    MaxStackSize = LoadedItem.MaxStackSize;
    ItemQuantity = LoadedItem.ItemQuantity;
    bIsEquippable = LoadedItem.bIsEquippable;
    ToolType = LoadedItem.ToolType;
    WeaponType = LoadedItem.WeaponType;
    ArmorType = LoadedItem.ArmorType;
    bHasDurability = LoadedItem.bHasDurability;
    MaxDurability = LoadedItem.MaxDurability;
    DurabilityDecayRate = LoadedItem.DurabilityDecayRate;
    bIsDestroyable = LoadedItem.bIsDestroyable;
    WeightClass = LoadedItem.WeightClass;
    bIsQuestItem = LoadedItem.bIsQuestItem;
    bIsUnique = LoadedItem.bIsUnique;
    
    InitialItemState = LoadedItem.InitialItemState;
    ItemState = LoadedItem.InitialItemState;
    
    DefaultModifiers = LoadedItem.DefaultModifiers;
    ItemModifiers = LoadedItem.DefaultModifiers;
    
    CurrentDurability = bHasDurability ? MaxDurability : 0;
}

bool FItemStructure::operator==(const FItemStructure& Other) const
{
    return RegistryKey == Other.RegistryKey &&
           ItemQuantity == Other.ItemQuantity &&
           ItemState == Other.ItemState;
}