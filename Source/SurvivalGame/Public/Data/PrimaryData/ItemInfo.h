// ItemInfo.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundBase.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "SurvivalGame/Public/Enums/ItemEnums.h"
#include "SurvivalGame/Public/Data/Struct/ItemStructure.h"
#include "ItemInfo.generated.h"

class UStaticMesh;
class USkeletalMesh;
class USoundBase;
class UParticleSystem;
class UTexture2D;

DECLARE_DYNAMIC_DELEGATE(FOnItemAssetsLoadedDelegate);

/**
 * @brief Primary Data Asset for defining static item properties
 * Used as a template for creating item instances in the game
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

    /** Asset Loading */
    UFUNCTION(BlueprintCallable, Category = "Item|Assets")
    void LoadItemAssets(const FOnItemAssetsLoadedDelegate& OnAssetsLoaded);

protected:
    /** Currently loaded assets */
    UPROPERTY(Transient)
    TArray<UObject*> LoadedAssets;

    /** Stored callback for Blueprint asset loading */
    FOnItemAssetsLoadedDelegate OnItemAssetsLoadedCallback;

    /** Called when assets are finished loading */
    void OnItemAssetsLoaded();

public:
    /** Core Properties */
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

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Core")
    E_ItemRarity ItemRarity;

    /** Stack Properties */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Stack")
    bool bIsStackable;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Stack", 
        meta = (EditCondition = "bIsStackable", EditConditionHides, ClampMin = "1"))
    int32 MaxStackSize;

    /** Visual Properties */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Visual", meta = (AllowedClasses = "StaticMesh"))
    TSoftObjectPtr<UStaticMesh> ItemMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Visual", meta = (AllowedClasses = "SkeletalMesh"))
    TSoftObjectPtr<USkeletalMesh> ItemSkeletalMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Visual", meta = (AllowedClasses = "ParticleSystem"))
    TSoftObjectPtr<UParticleSystem> ItemParticle;

    /** Audio Properties */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Audio", meta = (AllowedClasses = "SoundBase"))
    TSoftObjectPtr<USoundBase> ItemPickupSound;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Audio", meta = (AllowedClasses = "SoundBase"))
    TSoftObjectPtr<USoundBase> ItemUseSound;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Audio", meta = (AllowedClasses = "SoundBase"))
    TSoftObjectPtr<USoundBase> ItemDropSound;

    /** Equipment Properties */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Equipment")
    bool bIsEquippable;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Equipment", 
        meta = (EditCondition = "bIsEquippable", EditConditionHides))
    E_ToolType ToolType;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Equipment", 
        meta = (EditCondition = "bIsEquippable", EditConditionHides))
    E_WeaponType WeaponType;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Equipment", 
        meta = (EditCondition = "bIsEquippable", EditConditionHides))
    E_ArmorType ArmorType;

    /** Durability Properties */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Durability")
    bool bHasDurability;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Durability", 
        meta = (EditCondition = "bHasDurability", EditConditionHides, ClampMin = "1"))
    int32 MaxDurability;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Durability", 
        meta = (EditCondition = "bHasDurability", EditConditionHides, ClampMin = "0.0"))
    float DurabilityDecayRate;

    /** Consumable Properties */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Consumable")
    bool bIsConsumable;

    /** Physical Properties */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Physical")
    E_WeightClass WeightClass;

    /** Economic Properties */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Economic", meta = (ClampMin = "0"))
    int32 BaseValue;

    /** Special Properties */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Special")
    bool bIsQuestItem;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Special")
    bool bIsUnique;

    /** Modifiers */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Modifiers")
    TArray<FItemModifier> DefaultModifiers;
};