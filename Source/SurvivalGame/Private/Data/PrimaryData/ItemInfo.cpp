// ItemInfo.cpp

#include "SurvivalGame/Public/Data/PrimaryData/ItemInfo.h"

UItemInfo::UItemInfo()
{
    // Initialize Core Properties
    ItemType = E_ItemType::None;
    ItemCategory = E_ItemCategory::None;
    ItemRarity = E_ItemRarity::Common;

    // Initialize Stack Properties
    bIsStackable = false;
    MaxStackSize = 1;

    // Initialize Equipment Properties
    bIsEquippable = false;
    ToolType = E_ToolType::None;
    WeaponType = E_WeaponType::None;
    ArmorType = E_ArmorType::None;

    // Initialize Durability Properties
    bHasDurability = false;
    MaxDurability = 100;
    DurabilityDecayRate = 0.0f;

    // Initialize Consumable Properties
    bIsConsumable = false;

    // Initialize Physical Properties
    WeightClass = E_WeightClass::Light;

    // Initialize Special Properties
    bIsQuestItem = false;
    bIsUnique = false;

    // Initialize Economic Properties
    BaseValue = 0;
}

FPrimaryAssetId UItemInfo::GetPrimaryAssetId() const
{
    return FPrimaryAssetId(FPrimaryAssetType("Item"), GetFName());
}

FName UItemInfo::GetRegistryKey() const
{
    return FName(*FString::Printf(TEXT("ItemKey_%s"), *GetName()));
}

void UItemInfo::LoadItemAssets(const FOnItemAssetsLoadedDelegate& OnAssetsLoaded)
{
    OnItemAssetsLoadedCallback = OnAssetsLoaded;

    // Create array of assets to load
    TArray<FSoftObjectPath> AssetsToLoad;

    // Add all asset references that aren't already loaded
    auto AddAssetIfNotNull = [&AssetsToLoad](const TSoftObjectPtr<UObject>& Asset) {
        if (!Asset.IsNull()) {
            AssetsToLoad.Add(Asset.ToSoftObjectPath());
        }
    };

    AddAssetIfNotNull(ItemIcon);
    AddAssetIfNotNull(ItemMesh);
    AddAssetIfNotNull(ItemSkeletalMesh);
    AddAssetIfNotNull(ItemParticle);
    AddAssetIfNotNull(ItemPickupSound);
    AddAssetIfNotNull(ItemUseSound);
    AddAssetIfNotNull(ItemDropSound);

    if (AssetsToLoad.Num() > 0)
    {
        FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
        Streamable.RequestAsyncLoad(AssetsToLoad, FStreamableDelegate::CreateUObject(this, &UItemInfo::OnItemAssetsLoaded));
    }
    else
    {
        OnItemAssetsLoaded();
    }
}

void UItemInfo::OnItemAssetsLoaded()
{
    // Cache loaded assets
    LoadedAssets.Empty();

    auto CacheAssetIfNotNull = [this](const TSoftObjectPtr<UObject>& Asset) {
        if (!Asset.IsNull()) {
            LoadedAssets.Add(Asset.Get());
        }
    };

    CacheAssetIfNotNull(ItemIcon);
    CacheAssetIfNotNull(ItemMesh);
    CacheAssetIfNotNull(ItemSkeletalMesh);
    CacheAssetIfNotNull(ItemParticle);
    CacheAssetIfNotNull(ItemPickupSound);
    CacheAssetIfNotNull(ItemUseSound);
    CacheAssetIfNotNull(ItemDropSound);

    // Execute the Blueprint callback if bound
    if (OnItemAssetsLoadedCallback.IsBound())
    {
        OnItemAssetsLoadedCallback.Execute();
    }
}

FItemStructure UItemInfo::CreateItemInstance(int32 Quantity) const
{
    FItemStructure NewItem;

    // Set Core Properties
    FSoftObjectPath ItemPath(this);
    NewItem.ItemAsset = TSoftObjectPtr<UItemInfo>(ItemPath);
    NewItem.RegistryKey = GetRegistryKey();
    NewItem.ItemName = ItemName;
    NewItem.ItemType = ItemType;
    NewItem.ItemCategory = ItemCategory;
    NewItem.ItemRarity = ItemRarity;

    // Set Stack Properties
    NewItem.bIsStackable = bIsStackable;
    NewItem.MaxStackSize = MaxStackSize;
    NewItem.ItemQuantity = FMath::Clamp(Quantity, 1, bIsStackable ? MaxStackSize : 1);

    // Set Equipment Properties
    NewItem.bIsEquippable = bIsEquippable;
    if (bIsEquippable)
    {
        NewItem.ToolType = ToolType;

        // Assign Weapon or Armor Type if not a tool
        if (ToolType == E_ToolType::None)
        {
            if (WeaponType != E_WeaponType::None)
            {
                NewItem.WeaponType = WeaponType;
            }
            else if (ArmorType != E_ArmorType::None)
            {
                NewItem.ArmorType = ArmorType;
            }
            else
            {
                // Default behavior if all are None
                UE_LOG(LogTemp, Warning, TEXT("Equippable item has no valid Tool, Weapon, or Armor type!"));
            }
        }

        // Equipment automatically has durability
        NewItem.bHasDurability = true;
    }

    // Set Durability Properties
    NewItem.bHasDurability = bHasDurability || bIsEquippable;
    if (NewItem.bHasDurability)
    {
        NewItem.CurrentDurability = MaxDurability;
        NewItem.MaxDurability = MaxDurability;
        NewItem.DurabilityDecayRate = DurabilityDecayRate;
    }

    // Set Physical Properties
    NewItem.WeightClass = WeightClass;

    // Set Special Properties
    NewItem.bIsQuestItem = bIsQuestItem;
    NewItem.bIsUnique = bIsUnique;

    // Set Modifiers
    NewItem.DefaultModifiers = DefaultModifiers;
    NewItem.ItemModifiers = DefaultModifiers;

    // Set Initial State
    NewItem.InitialItemState = E_ItemState::Normal;
    NewItem.ItemState = E_ItemState::Normal;

    return NewItem;
}
