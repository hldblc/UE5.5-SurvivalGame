// ItemStructure.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SurvivalGame/Public/Enums/ItemEnums.h"
#include "ItemStructure.generated.h"

class UItemInfo;

/**
 * @brief Structure representing runtime item data
 */
// ItemStructure.h
USTRUCT(BlueprintType)
struct SURVIVALGAME_API FItemStructure : public FTableRowBase
{
    GENERATED_BODY()

    /** Core Properties */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Core")
    FName RegistryKey;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Core")
    TSoftObjectPtr<UItemInfo> ItemAsset;

    /** Item Properties */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    int32 ItemQuantity;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    int32 StackSize;

    /** Stats */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float CurrentHP;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MaxHP;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    int32 CurrentAmmo;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    int32 MaxAmmo;

    /** Constructor */
    FItemStructure();

    /** Basic utility function */
    bool IsEmpty() const { return RegistryKey.IsNone(); }
};