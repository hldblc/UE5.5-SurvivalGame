// ItemStructure.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SurvivalGame/Public/Enums/ItemEnums.h"
#include "ItemStructure.generated.h"

class UItemInfo;

/**
 * @brief Structure representing an item modifier
 */
USTRUCT(BlueprintType)
struct FItemModifier
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifier")
    FString ModifierName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifier")
    float ModifierValue;

    FItemModifier() : ModifierValue(0.0f) {}
};

/**
 * @brief Structure representing runtime item data
 */
USTRUCT(BlueprintType, Blueprintable)
struct SURVIVALGAME_API FItemStructure : public FTableRowBase
{
    GENERATED_BODY()

    /** Core Properties */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Core")
    FName RegistryKey;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Core")
    TSoftObjectPtr<UItemInfo> ItemAsset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Core")
    FText ItemName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Core")
    E_ItemType ItemType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Core")
    E_ItemCategory ItemCategory;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Core")
    E_ItemRarity ItemRarity;

    /** Stack Properties */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stack")
    bool bIsStackable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stack", 
        meta = (ClampMin = "1", EditCondition = "bIsStackable"))
    int32 MaxStackSize;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stack")
    int32 ItemQuantity;

    /** Equipment Properties */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
    bool bIsEquippable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment",
        meta = (EditCondition = "bIsEquippable"))
    E_ToolType ToolType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment",
        meta = (EditCondition = "bIsEquippable"))
    E_WeaponType WeaponType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment",
        meta = (EditCondition = "bIsEquippable"))
    E_ArmorType ArmorType;

    /** Durability Properties */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Durability")
    bool bHasDurability;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Durability",
        meta = (EditCondition = "bHasDurability"))
    int32 CurrentDurability;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Durability",
        meta = (EditCondition = "bHasDurability", ClampMin = "1"))
    int32 MaxDurability;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Durability",
        meta = (EditCondition = "bHasDurability", ClampMin = "0.0"))
    float DurabilityDecayRate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Durability")
    bool bIsDestroyable;

    /** Physical Properties */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physical")
    E_WeightClass WeightClass;

    /** Special Properties */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Special")
    bool bIsQuestItem;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Special")
    bool bIsUnique;

    /** Modifiers */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifiers")
    TArray<FItemModifier> DefaultModifiers;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifiers")
    TArray<FItemModifier> ItemModifiers;

    /** State Properties */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
    E_ItemState InitialItemState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
    E_ItemState ItemState;

    /** Constructor */
    FItemStructure();

    /** Initialize from another structure */
    void InitializeFromData(const FItemStructure& LoadedItem);

    /** Get current condition as percentage */
    FORCEINLINE float GetConditionPercent() const
    {
        return MaxDurability > 0 ? (float)CurrentDurability / (float)MaxDurability : 1.0f;
    }

    /** Check if item is empty/invalid */
    FORCEINLINE bool IsEmpty() const
    {
        return RegistryKey.IsNone();
    }

    /** Check if the item can be stacked with another item */
    FORCEINLINE bool CanStack(const FItemStructure& Other) const
    {
        return bIsStackable && Other.bIsStackable && 
               RegistryKey == Other.RegistryKey && 
               ItemState == Other.ItemState;
    }

    /** Get remaining space in the stack */
    FORCEINLINE int32 GetRemainingStackSpace() const
    {
        return bIsStackable ? MaxStackSize - ItemQuantity : 0;
    }

    /** Check if the item is damaged */
    FORCEINLINE bool IsDamaged() const
    {
        return bHasDurability && CurrentDurability < MaxDurability;
    }

    /** Check if the item is broken */
    FORCEINLINE bool IsBroken() const
    {
        return bHasDurability && CurrentDurability <= 0;
    }

    /** Equality operator */
    bool operator==(const FItemStructure& Other) const;
};