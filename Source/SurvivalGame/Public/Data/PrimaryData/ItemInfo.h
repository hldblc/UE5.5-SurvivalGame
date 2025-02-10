// ItemInfo.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SurvivalGame/Public/Enums/ItemEnums.h"
#include "SurvivalGame/Public/Data/Struct/ItemStructure.h"
#include "ItemInfo.generated.h"

class UTexture2D;

/**
 * @brief Primary Data Asset for defining static item properties
 */
UCLASS(BlueprintType, Blueprintable)
class SURVIVALGAME_API UItemInfo : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UItemInfo();

    /** Core Functions */
    virtual FPrimaryAssetId GetPrimaryAssetId() const override;
    virtual FName GetRegistryKey() const;

    /** Creates a new instance of this item with runtime data */
    UFUNCTION(BlueprintCallable, Category = "Item")
    FItemStructure CreateItemInstance(int32 Quantity = 1) const;

public:
    /** Core Properties */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Core")
    FName RegistryKey;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Core", meta = (AllowedClasses = "Texture2D"))
    TSoftObjectPtr<UTexture2D> ItemIcon;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Core")
    FText ItemName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Core", meta = (MultiLine = true))
    FText ItemDescription;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Core")
    E_ItemType ItemType;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Core")
    E_ItemCategory ItemCategory;
};