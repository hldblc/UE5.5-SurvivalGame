// ItemEnums.h

#pragma once

#include "CoreMinimal.h"
#include "ItemEnums.generated.h"

/**
 * @brief Primary item classification
 */
UENUM(BlueprintType)
enum class E_ItemType : uint8
{
    None            UMETA(DisplayName = "None"),
    Weapon          UMETA(DisplayName = "Weapon"),
    Armor           UMETA(DisplayName = "Armor"),
    Tool            UMETA(DisplayName = "Tool"),
    Resource        UMETA(DisplayName = "Resource"),
    Food            UMETA(DisplayName = "Food"),
    Potion          UMETA(DisplayName = "Potion"),
    QuestItem       UMETA(DisplayName = "Quest Item"),
    Valuable        UMETA(DisplayName = "Valuable"),
    Miscellaneous   UMETA(DisplayName = "Miscellaneous")    

};

/**
 * @brief Specific item categories within types
 */
UENUM(BlueprintType)
enum class E_ItemCategory : uint8
{
    None        UMETA(DisplayName = "None"),
    Resource    UMETA(DisplayName = "Resource"),
    Equipment   UMETA(DisplayName = "Equipable"),
    Consumable  UMETA(DisplayName = "Consumable"),
    Buildable   UMETA(DisplayName = "Buildable"),
    Quest       UMETA(DisplayName = "Quest"),
    Miscellaneous UMETA(DisplayName = "Miscellaneous")
};

//////////////////////////////////////////////////
/// Armor Type Enums
//////////////////////////////////////////////////

UENUM(BlueprintType)
enum class E_ArmorType : uint8
{
    None              UMETA(DisplayName = "None"),
    Helmet            UMETA(DisplayName = "Helmet"),
    Chestplate        UMETA(DisplayName = "Chestplate"),
    Leggings          UMETA(DisplayName = "Leggings"),
    Boots             UMETA(DisplayName = "Boots"),
    Shield            UMETA(DisplayName = "Shield"),
    Invalid           UMETA(HIDDEN)
    
};

/**
 * @brief Item rarity affecting value and stats
 */
UENUM(BlueprintType)
enum class E_ItemRarity : uint8
{
    None        UMETA(DisplayName = "None"),
    Common      UMETA(DisplayName = "Common"),
    Uncommon    UMETA(DisplayName = "Uncommon"),
    Rare        UMETA(DisplayName = "Rare"),
    Epic        UMETA(DisplayName = "Epic"),
    Legendary   UMETA(DisplayName = "Legendary")
};

/**
 * @brief Equipment slots for wearable items
 */
UENUM(BlueprintType)
enum class E_EquipmentSlot : uint8
{
    None        UMETA(DisplayName = "None"),
    Head        UMETA(DisplayName = "Head"),
    Chest       UMETA(DisplayName = "Chest"),
    Legs        UMETA(DisplayName = "Legs"),
    Feet        UMETA(DisplayName = "Feet"),
    Hands       UMETA(DisplayName = "Hands"),
    MainHand    UMETA(DisplayName = "Main Hand"),
    OffHand     UMETA(DisplayName = "Off Hand")
};

/**
 * @brief Weight class affecting movement and stamina
 */
UENUM(BlueprintType)
enum class E_WeightClass : uint8
{
    None    UMETA(DisplayName = "None"),
    Light   UMETA(DisplayName = "Light"),
    Medium  UMETA(DisplayName = "Medium"),
    Heavy   UMETA(DisplayName = "Heavy")
};

/**
 * @brief Types of damage for weapons
 */
UENUM(BlueprintType)
enum class E_DamageType : uint8
{
    None        UMETA(DisplayName = "None"),
    Physical    UMETA(DisplayName = "Physical"),
    Fire        UMETA(DisplayName = "Fire"),
    Ice         UMETA(DisplayName = "Ice"),
    Lightning   UMETA(DisplayName = "Lightning"),
    Magic       UMETA(DisplayName = "Magic")
};

/**
 * @brief Weapon classifications
 */
UENUM(BlueprintType)
enum class E_WeaponType : uint8
{
    None        UMETA(DisplayName = "None"),
    Sword       UMETA(DisplayName = "Sword"),
    Axe         UMETA(DisplayName = "Axe"),
    Spear       UMETA(DisplayName = "Spear"),
    Bow         UMETA(DisplayName = "Bow"),
    Staff       UMETA(DisplayName = "Staff"),
    Shield      UMETA(DisplayName = "Shield")
};

/**
 * @brief Tool classifications
 */
UENUM(BlueprintType)
enum class E_ToolType : uint8
{
    None        UMETA(DisplayName = "None"),
    Pickaxe     UMETA(DisplayName = "Pickaxe"),
    Hatchet     UMETA(DisplayName = "Hatchet"),
    Shovel      UMETA(DisplayName = "Shovel"),
    FishingRod  UMETA(DisplayName = "")
};

/**
 * @brief Resource classifications
 */
UENUM(BlueprintType)
enum class E_ResourceType : uint8
{
    None        UMETA(DisplayName = "None"),
    Wood        UMETA(DisplayName = "Wood"),
    Stone       UMETA(DisplayName = "Stone"),
    Metal       UMETA(DisplayName = "Metal"),
    Herb        UMETA(DisplayName = "Herb"),
    Hide        UMETA(DisplayName = "Hide"),
    Gem         UMETA(DisplayName = "Gem")
};

/**
 * @brief Item effects when used or equipped
 */
UENUM(BlueprintType)
enum class E_ItemEffect : uint8
{
    None            UMETA(DisplayName = "None"),
    RestoreHealth   UMETA(DisplayName = "Restore Health"),
    RestoreStamina  UMETA(DisplayName = "Restore Stamina"),
    BoostDefense    UMETA(DisplayName = "Boost Defense"),
    BoostDamage     UMETA(DisplayName = "Boost Damage"),
    BoostSpeed      UMETA(DisplayName = "Boost Speed"),
    ResistFire      UMETA(DisplayName = "Resist Fire"),
    ResistIce       UMETA(DisplayName = "Resist Ice"),
    ResistPoison    UMETA(DisplayName = "Resist Poison")
};

/**
 * @brief Inventory sorting methods
 */
UENUM(BlueprintType)
enum class E_SortMethod : uint8
{
    None        UMETA(DisplayName = "None"),
    Name        UMETA(DisplayName = "Name"),
    Type        UMETA(DisplayName = "Type"),
    Rarity      UMETA(DisplayName = "Rarity"),
    Value       UMETA(DisplayName = "Value"),
    Weight      UMETA(DisplayName = "Weight")
};

/**
 * @brief Resource state affecting gathering and quality
 */
UENUM(BlueprintType)
enum class E_ResourceState : uint8
{
    None            UMETA(DisplayName = "None"),
    Bountiful       UMETA(DisplayName = "Bountiful"),
    Standard        UMETA(DisplayName = "Standard"),
    Scarce          UMETA(DisplayName = "Scarce"),
    Exhausted       UMETA(DisplayName = "Exhausted"),
    Cursed          UMETA(DisplayName = "Cursed")
};

/**
 * @brief Equipable state affecting functionality and durability
 */
UENUM(BlueprintType)
enum class E_EquipableState : uint8
{
    None            UMETA(DisplayName = "None"),
    Normal          UMETA(DisplayName = "Normal"),
    Damaged         UMETA(DisplayName = "Damaged"),
    Broken          UMETA(DisplayName = "Broken"),
    Cursed          UMETA(DisplayName = "Cursed")
};

/**
 * @brief Consumable state affecting effectiveness and safety
 */
UENUM(BlueprintType)
enum class E_ConsumableState : uint8
{
    None            UMETA(DisplayName = "None"),
    Fresh           UMETA(DisplayName = "Fresh"),
    Normal          UMETA(DisplayName = "Normal"),
    Raw             UMETA(DisplayName = "Raw"),
    Processed       UMETA(DisplayName = "Processed"),
    Spoiled         UMETA(DisplayName = "Spoiled"),
    Poisoned        UMETA(DisplayName = "Poisoned")
};

/**
 * @brief Buildable state for structures and settlements
 */
UENUM(BlueprintType)
enum class E_BuildableState : uint8
{
    None               UMETA(DisplayName = "None"),
    Planned            UMETA(DisplayName = "Planned"),
    UnderConstruction  UMETA(DisplayName = "Under Construction"),
    Completed          UMETA(DisplayName = "Completed"),
    Damaged            UMETA(DisplayName = "Damaged"),
    Destroyed          UMETA(DisplayName = "Destroyed"),
    Ruined             UMETA(DisplayName = "Ruined")
};