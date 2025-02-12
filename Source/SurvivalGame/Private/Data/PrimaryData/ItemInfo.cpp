#include "Data/PrimaryData/ItemInfo.h"
#include "Engine/Texture2D.h"

UItemInfo::UItemInfo()
{
    // Set default values (adjust as needed)
    RegistryKey = FName("DefaultItem");
    ItemCategory = E_ItemCategory::None;
    ItemRarity = E_ItemRarity::Common;
    ItemDamage = 0;
    bStackable = false;
    StackSize = 1;
    ItemClassRef = nullptr;
    ArmorType = E_ArmorType::None; // Assumes an enum value "None" exists
    bUseAmmo = false;
    CurrAmmo = 0;
    MaxAmmo = 0;
    ItemCurHP = 100;
    ItemMaxHP = 100;
    ItemWeight = 0.0f;

#if WITH_EDITOR
    // Initialize helper booleans.
    bShowAmmo = false;
    bShowArmorType = false;
    bShowDamage = false;
#endif
}

FPrimaryAssetId UItemInfo::GetPrimaryAssetId() const
{
    // Use "Item" as the asset type and the RegistryKey as the identifier.
    return FPrimaryAssetId(FPrimaryAssetType("Item"), RegistryKey);
}

FName UItemInfo::GetRegistryKey() const
{
    return RegistryKey;
}

FItemStructure UItemInfo::CreateItemInstance(int32 Quantity) const
{
    FItemStructure NewItem;
    // Core properties.
    NewItem.RegistryKey = RegistryKey;
    NewItem.ItemAsset = TSoftObjectPtr<UItemInfo>(FSoftObjectPath(GetPathName()));

    // Item properties.
    NewItem.ItemQuantity = Quantity;
    NewItem.StackSize = StackSize;

    // Stats.
    NewItem.CurrentHP = static_cast<float>(ItemCurHP);
    NewItem.MaxHP = static_cast<float>(ItemMaxHP);
    NewItem.CurrentAmmo = CurrAmmo;
    NewItem.MaxAmmo = MaxAmmo;

    return NewItem;
}

#if WITH_EDITOR
void UItemInfo::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    // Determine which property changed.
    FName PropertyName = (PropertyChangedEvent.Property != nullptr)
                             ? PropertyChangedEvent.Property->GetFName()
                             : NAME_None;

    // When ItemCategory, bUseAmmo, or bStackable change, update the helper booleans.
    if (PropertyName == GET_MEMBER_NAME_CHECKED(UItemInfo, ItemCategory) ||
        PropertyName == GET_MEMBER_NAME_CHECKED(UItemInfo, bUseAmmo) ||
        PropertyName == GET_MEMBER_NAME_CHECKED(UItemInfo, bStackable))
    {
        // Example conditions:
        // For a weapon, show ammo information if bUseAmmo is true.
        bShowAmmo = (ItemCategory == E_ItemCategory::Weapon) && bUseAmmo;

        // For armor, show the ArmorType field.
        bShowArmorType = (ItemCategory == E_ItemCategory::Armor);

        // For damage, show ItemDamage for weapons (adjust logic as needed).
        bShowDamage = (ItemCategory == E_ItemCategory::Weapon);
    }
}
#endif
