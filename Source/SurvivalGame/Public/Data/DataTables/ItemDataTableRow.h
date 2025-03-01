// ItemDataTableRow.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemEnums.h"
#include "ItemDataTableRow.generated.h"

USTRUCT(BlueprintType)
struct FItemDataTableRow : public FTableRowBase
{
    GENERATED_BODY()

    // References
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Core")
    TSoftObjectPtr<UTexture2D> ItemIcon;
    
    // Core Properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Core")
    FName RegistryKey;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Core")
    E_ItemCategory ItemCategory;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Core")
    E_ItemType ItemType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Core")
    FText ItemName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Core")
    FText ItemDescription;

    // Stats
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float ItemWeight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    int32 ItemBaseHP;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    int32 ItemDamage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    bool bStackable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    int32 StackSize;

    FItemDataTableRow()
       : RegistryKey(NAME_None)
       , ItemCategory(E_ItemCategory::None)
       , ItemType(E_ItemType::None)
       , ItemWeight(0.0f)
       , ItemBaseHP(100)
       , ItemDamage(0)
       , bStackable(false)
       , StackSize(1)
    {}

    // Getter functions
    FORCEINLINE UTexture2D* GetItemIcon() const { return ItemIcon.LoadSynchronous(); }
    FORCEINLINE FName GetRegistryKey() const { return RegistryKey; }
    FORCEINLINE E_ItemCategory GetItemCategory() const { return ItemCategory; }
    FORCEINLINE E_ItemType GetItemType() const { return ItemType; }
    FORCEINLINE FText GetItemName() const { return ItemName; }
    FORCEINLINE FText GetItemDescription() const { return ItemDescription; }
    FORCEINLINE float GetItemWeight() const { return ItemWeight; }
    FORCEINLINE int32 GetItemBaseHP() const { return ItemBaseHP; }
    FORCEINLINE int32 GetItemDamage() const { return ItemDamage; }
    FORCEINLINE bool IsStackable() const { return bStackable; }
    FORCEINLINE int32 GetStackSize() const { return StackSize; }
    
    // Utility functions
    FORCEINLINE bool IsValid() const { return !RegistryKey.IsNone(); }
    FORCEINLINE bool CanStack(const FItemDataTableRow& OtherItem) const
    {
        return bStackable && OtherItem.bStackable && RegistryKey == OtherItem.RegistryKey;
    }
    
    // Get the stack limit for this item
    FORCEINLINE int32 GetMaxStackSize() const { return bStackable ? StackSize : 1; }
    
    // Check if item is a specific category
    FORCEINLINE bool IsWeapon() const { return ItemCategory == E_ItemCategory::Equipment; }
    FORCEINLINE bool IsArmor() const { return ItemCategory == E_ItemCategory::Wearable; }
    FORCEINLINE bool IsConsumable() const { return ItemCategory == E_ItemCategory::Consumable || ItemType == E_ItemType::Food || ItemType == E_ItemType::Potion; }
    FORCEINLINE bool IsResource() const { return ItemCategory == E_ItemCategory::Resource; }
    
    // Get formatted information string
    FString GetItemInfoString() const
    {
        FString Result = FString::Printf(TEXT("%s\n"), *ItemName.ToString());
        Result += FString::Printf(TEXT("Type: %s\n"), *UEnum::GetDisplayValueAsText(ItemCategory).ToString());
        Result += FString::Printf(TEXT("Weight: %.1f\n"), ItemWeight);
        
        if (ItemDamage > 0)
        {
            Result += FString::Printf(TEXT("Damage: %d\n"), ItemDamage);
        }
        
        if (bStackable)
        {
            Result += FString::Printf(TEXT("Stack Size: %d\n"), StackSize);
        }
        
        Result += FString::Printf(TEXT("\n%s"), *ItemDescription.ToString());
        return Result;
    }
};