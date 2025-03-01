#include "Data/PrimaryData/ItemInfo.h"

#include "DataTables/ItemDataTableRow.h"
#include "Engine/Texture2D.h"

UItemInfo::UItemInfo()
{
    // Set default values
    RegistryKey = FName("DefaultItem");
    ItemCategory = E_ItemCategory::None;
    ItemType = E_ItemType::None;
    ItemRarity = E_ItemRarity::Common;
    ItemDamage = 0;
    bStackable = false;
    StackSize = 1;
    ItemClassRef = nullptr;
    ArmorType = E_ArmorType::None;
    bUseAmmo = false;
    CurrAmmo = 0;
    MaxAmmo = 0;
    ItemBaseHP = 100;
    ItemCurHP = 100;
    ItemWeight = 0.0f;
    WeaponType = E_WeaponType::None;
    ToolType = E_ToolType::None;
    ResourceType = E_ResourceType::None;
    EquipmentSlot = E_EquipmentSlot::None;
    
    // Initialize state values
    EquipableState = E_EquipableState::Normal;
    ResourceState = E_ResourceState::Standard;
    ConsumableState = E_ConsumableState::Normal;
    BuildableState = E_BuildableState::Completed;

#if WITH_EDITOR
    // Initialize helper booleans
    bShowAmmo = false;
    bShowArmorType = false;
    bShowDamage = false;
    bShowWeaponType = false;
    bShowToolType = false;
    bShowResourceType = false;
    bShowEquipmentSlot = false;
    bShowEquipableState = false;
    bShowResourceState = false;
    bShowConsumableState = false;
    bShowBuildableState = false;
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

int32 UItemInfo::GetMaxHP() const
{
    // Calculate max HP based on item category and state
    switch (ItemCategory)
    {
        case E_ItemCategory::Equipment:
        {
            // Equipable items have state-based max HP
            switch (EquipableState)
            {
                case E_EquipableState::Normal:
                    return ItemBaseHP;
                case E_EquipableState::Damaged:
                    return static_cast<int32>(ItemBaseHP * 0.8f); // 80% of max HP
                case E_EquipableState::Broken:
                    return static_cast<int32>(ItemBaseHP * 0.5f); // 50% of max HP
                case E_EquipableState::Cursed:
                    return static_cast<int32>(ItemBaseHP * 1.2f); // 120% of max HP but with other drawbacks
                default:
                    return ItemBaseHP;
            }
        }
        
        case E_ItemCategory::Consumable:
        {
            // Consumable items have state-based effectiveness
            switch (ConsumableState)
            {
                case E_ConsumableState::Fresh:
                    return static_cast<int32>(ItemBaseHP * 1.2f); // 120% effectiveness
                case E_ConsumableState::Normal:
                    return ItemBaseHP;
                case E_ConsumableState::Raw:
                    return static_cast<int32>(ItemBaseHP * 0.75f); // 75% effectiveness
                case E_ConsumableState::Processed:
                    return static_cast<int32>(ItemBaseHP * 1.1f); // 110% effectiveness
                case E_ConsumableState::Spoiled:
                    return static_cast<int32>(ItemBaseHP * 0.4f); // 40% effectiveness
                case E_ConsumableState::Poisoned:
                    return 0; // Harmful
                default:
                    return ItemBaseHP;
            }
        }
        
        default:
            return ItemBaseHP;
    }
}

bool UItemInfo::LoadFromDataTable(const UDataTable* DataTable)
{
    if (!DataTable || DataTableRowName.IsNone())
    {
        return false;
    }

    FItemDataTableRow* Row = DataTable->FindRow<FItemDataTableRow>(DataTableRowName, TEXT(""));
    if (!Row)
    {
        return false;
    }

    // Copy data from table row to this asset
    RegistryKey = Row->RegistryKey;
    ItemCategory = Row->ItemCategory;
    ItemType = Row->ItemType;
    ItemName = Row->ItemName;
    ItemDescription = Row->ItemDescription;
    ItemWeight = Row->ItemWeight;
    ItemBaseHP = Row->ItemBaseHP;
    ItemDamage = Row->ItemDamage;
    bStackable = Row->bStackable;
    StackSize = Row->StackSize;
    ItemIcon = Row->ItemIcon;

    return true;
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

    // Stats - use the state-based HP calculation
    NewItem.CurrentHP = static_cast<float>(ItemCurHP);
    NewItem.MaxHP = static_cast<float>(GetMaxHP());
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

    // Update helper booleans when relevant properties change
    if (PropertyName == GET_MEMBER_NAME_CHECKED(UItemInfo, ItemCategory) ||
        PropertyName == GET_MEMBER_NAME_CHECKED(UItemInfo, ItemType) ||
        PropertyName == GET_MEMBER_NAME_CHECKED(UItemInfo, bUseAmmo) ||
        PropertyName == GET_MEMBER_NAME_CHECKED(UItemInfo, bStackable))
    {
        // Reset all conditional flags first
        bShowAmmo = false;
        bShowArmorType = false; 
        bShowDamage = false;
        bShowWeaponType = false;
        bShowToolType = false;
        bShowResourceType = false;
        bShowEquipmentSlot = false;
        bShowEquipableState = false;
        bShowResourceState = false;
        bShowConsumableState = false;
        bShowBuildableState = false;

        // Equipment type-specific properties
        if (ItemCategory == E_ItemCategory::Equipment)
        {
            bShowEquipableState = true;
            bShowEquipmentSlot = true;
            
            // Weapon-specific properties
            if (ItemType == E_ItemType::Weapon)
            {
                bShowWeaponType = true;
                bShowDamage = true;
                bShowAmmo = bUseAmmo;
            }
            // Tool-specific properties
            else if (ItemType == E_ItemType::Tool)
            {
                bShowToolType = true;
                bShowDamage = true;
            }
            // Armor-specific properties
            else if (ItemType == E_ItemType::Armor)
            {
                bShowArmorType = true;
            }
        }
        // Resource-specific properties
        else if (ItemCategory == E_ItemCategory::Resource)
        {
            bShowResourceType = true;
            bShowResourceState = true;
        }
        // Consumable-specific properties
        else if (ItemCategory == E_ItemCategory::Consumable)
        {
            bShowConsumableState = true;
            if (ItemType == E_ItemType::Food || ItemType == E_ItemType::Potion)
            {
                bShowDamage = true; // For damage-boosting consumables
            }
        }
        // Quest items don't have special states
        
        // Buildable items
        else if (ItemCategory == E_ItemCategory::Miscellaneous && 
                 (ItemType == E_ItemType::None || ItemType == E_ItemType::Miscellaneous))
        {
            bShowBuildableState = true;
        }
        
        // Auto-set stackable based on category/type combinations
        if (PropertyName == GET_MEMBER_NAME_CHECKED(UItemInfo, ItemCategory) ||
            PropertyName == GET_MEMBER_NAME_CHECKED(UItemInfo, ItemType))
        {
            // Resources and consumables are typically stackable
            if (ItemCategory == E_ItemCategory::Resource || 
                ItemCategory == E_ItemCategory::Consumable)
            {
                bStackable = true;
                StackSize = 20; // Default stack size
            }
            // Equipment is typically not stackable
            else if (ItemCategory == E_ItemCategory::Equipment)
            {
                bStackable = false;
                StackSize = 1;
            }
        }
    }
    
    // Update equipment slot visibility based on specific weapon/armor type
    if (PropertyName == GET_MEMBER_NAME_CHECKED(UItemInfo, WeaponType))
    {
        // Different weapons go in different slots
        if (WeaponType == E_WeaponType::Shield)
        {
            EquipmentSlot = E_EquipmentSlot::OffHand;
        }
        else if (WeaponType != E_WeaponType::None)
        {
            EquipmentSlot = E_EquipmentSlot::MainHand;
        }
    }
    else if (PropertyName == GET_MEMBER_NAME_CHECKED(UItemInfo, ArmorType))
    {
        // Map armor types to the appropriate equipment slots
        switch (ArmorType)
        {
            case E_ArmorType::Helmet:
                EquipmentSlot = E_EquipmentSlot::Head;
                break;
            case E_ArmorType::Chestplate:
                EquipmentSlot = E_EquipmentSlot::Chest;
                break;
            case E_ArmorType::Leggings:
                EquipmentSlot = E_EquipmentSlot::Legs;
                break;
            case E_ArmorType::Boots:
                EquipmentSlot = E_EquipmentSlot::Feet;
                break;
            case E_ArmorType::Shield:
                EquipmentSlot = E_EquipmentSlot::OffHand;
                break;
            default:
                break;
        }
    }
    
    // Update health values when state changes
    if (PropertyName == GET_MEMBER_NAME_CHECKED(UItemInfo, EquipableState) ||
        PropertyName == GET_MEMBER_NAME_CHECKED(UItemInfo, ConsumableState) ||
        PropertyName == GET_MEMBER_NAME_CHECKED(UItemInfo, ResourceState) ||
        PropertyName == GET_MEMBER_NAME_CHECKED(UItemInfo, BuildableState))
    {
        // If in editor, update the current HP based on state for preview purposes
        ItemCurHP = GetMaxHP();
    }
}
#endif