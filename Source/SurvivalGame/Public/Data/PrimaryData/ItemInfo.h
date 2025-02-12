#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SurvivalGame/Public/Enums/ItemEnums.h"
#include "SurvivalGame/Public/Data/Struct/ItemStructure.h"
#include "ItemInfo.generated.h"

class UTexture2D;
class AItemMaster; // Forward declaration of your ItemMaster class

/**
 * @brief Primary Data Asset for defining static item properties.
 *
 * This version uses helper booleans and EditCondition metadata to conditionally show/hide properties
 * based on the selected ItemCategory (e.g. showing ammo fields for weapons, armor type for armor, etc.).
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
    E_ItemCategory ItemCategory;

    /** Additional Properties */

    // Damage: Only visible when the item is damageable (for example, when the category is Weapon).
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Stats", meta = (EditCondition = "bShowDamage", EditConditionHides))
    int32 ItemDamage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Stats")
    E_ItemRarity ItemRarity;

    // Stackable flag; used directly for showing the StackSize property.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Stats")
    bool bStackable;

    // StackSize is only shown when bStackable is true.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Stats", meta = (EditCondition = "bStackable", EditConditionHides))
    int32 StackSize;

    // Soft class reference for the item master class
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Class")
    TSoftClassPtr<AItemMaster> ItemClassRef;

    // Armor type is shown only when the item category is Armor.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Stats", meta = (EditCondition = "bShowArmorType", EditConditionHides))
    E_ArmorType ArmorType;

    // Ammo usage flag.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Stats")
    bool bUseAmmo;

    // Ammo values are shown only when the item category is Weapon and bUseAmmo is true.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Stats", meta = (EditCondition = "bShowAmmo", EditConditionHides))
    int32 CurrAmmo;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Stats", meta = (EditCondition = "bShowAmmo", EditConditionHides))
    int32 MaxAmmo;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Stats")
    int32 ItemCurHP;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Stats")
    int32 ItemMaxHP;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Stats")
    float ItemWeight;

#if WITH_EDITORONLY_DATA
    // Editor-only helper booleans for conditional display.
    // These are updated in PostEditChangeProperty based on ItemCategory and other flags.
    UPROPERTY(Transient, VisibleAnywhere, Category = "Item|Conditional")
    bool bShowAmmo;

    UPROPERTY(Transient, VisibleAnywhere, Category = "Item|Conditional")
    bool bShowArmorType;

    UPROPERTY(Transient, VisibleAnywhere, Category = "Item|Conditional")
    bool bShowDamage;
#endif

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
