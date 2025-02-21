// ItemContainerBase.cpp

#include "Components/Inventory/ItemContainerBase.h"

#include "Engine/AssetManager.h"
#include "Engine/ObjectLibrary.h"
#include "Interfaces/PlayerInterface.h"
#include "Net/UnrealNetwork.h"
#include "PrimaryData/ItemInfo.h"

UItemContainerBase::UItemContainerBase()
{
    // Disable tick by default for performance
    PrimaryComponentTick.bCanEverTick = false;
    
    // Enable replication
    SetIsReplicatedByDefault(true);

    // Default configuration
    MaxSlots = 20;
    ContainerType = E_ContainerType::Storage;
}

void UItemContainerBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // Replicate items array to all clients
    DOREPLIFETIME(UItemContainerBase, Items);
}

void UItemContainerBase::BeginPlay()
{
    Super::BeginPlay();
    
    // Cache owner reference
    OwningActor = GetOwner();
    
    // Initialize container slots
    InitializeContainer();
}




void UItemContainerBase::DebugContainerState()
{
    UE_LOG(LogTemp, Log, TEXT("=== CONTAINER DEBUG: %s ==="), *GetName());
    
    // Fix the enum value lookup using StaticEnum
    const UEnum* ContainerEnum = StaticEnum<E_ContainerType>();
    if (ContainerEnum)
    {
        UE_LOG(LogTemp, Log, TEXT("Container type: %s"), *ContainerEnum->GetNameStringByValue(static_cast<uint8>(ContainerType)));
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("Container type: %d (enum not found)"), static_cast<uint8>(ContainerType));
    }
    
    UE_LOG(LogTemp, Log, TEXT("Max slots: %d"), MaxSlots);
    UE_LOG(LogTemp, Log, TEXT("Current items: %d"), Items.Num());
    
    for (int32 i = 0; i < Items.Num(); i++)
    {
        if (!Items[i].RegistryKey.IsNone())
        {
            UE_LOG(LogTemp, Log, TEXT("[%d] Item: %s"), i, *Items[i].RegistryKey.ToString());
            UE_LOG(LogTemp, Log, TEXT("    Asset: %s"), 
                !Items[i].ItemAsset.IsNull() ? *Items[i].ItemAsset.ToSoftObjectPath().ToString() : TEXT("NULL"));
            UE_LOG(LogTemp, Log, TEXT("    Quantity: %d/%d"), Items[i].ItemQuantity, Items[i].StackSize);
            UE_LOG(LogTemp, Log, TEXT("    HP: %.1f/%.1f"), Items[i].CurrentHP, Items[i].MaxHP);
        }
    }
    
    // Check if AssetManager is working properly
    UAssetManager* AssetManager = UAssetManager::GetIfInitialized();
    if (AssetManager)
    {
        TArray<FPrimaryAssetId> ItemIds;
        AssetManager->GetPrimaryAssetIdList(FPrimaryAssetType("Item"), ItemIds);
        UE_LOG(LogTemp, Log, TEXT("AssetManager knows about %d 'Item' assets"), ItemIds.Num());
        
        // Log first few items for debugging
        const int32 MaxItemsToLog = FMath::Min(ItemIds.Num(), 5);
        for (int32 i = 0; i < MaxItemsToLog; i++)
        {
            UE_LOG(LogTemp, Log, TEXT("  - Item[%d]: %s"), i, *ItemIds[i].ToString());
            
            // Try to get the asset path
            FSoftObjectPath AssetPath = AssetManager->GetPrimaryAssetPath(ItemIds[i]);
            if (!AssetPath.IsNull())
            {
                UE_LOG(LogTemp, Log, TEXT("      Path: %s"), *AssetPath.ToString());
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("AssetManager not initialized!"));
    }
}




void UItemContainerBase::InitializeContainer()
{
    if (GetOwnerRole() == ROLE_Authority)
    {
        // Initialize with empty slots
        Items.SetNum(MaxSlots);
        for (int32 i = 0; i < MaxSlots; ++i)
        {
            Items[i] = FItemStructure(); // Creates empty item structure
        }
    }
}

//===========================================Server_AddItem====================================================
void UItemContainerBase::Server_AddItem_Implementation(const FItemStructure& Item)
{
    AddItem(Item);
}

//===========================================UpdateUI====================================================


void UItemContainerBase::UpdateUI(const int32 Index, const FItemStructure& ItemInfo)
{
    AActor* OwnerActor = GetOwner();
    if (!OwnerActor)
    {
        return;
    }

    switch (ContainerType)
    {
    case E_ContainerType::Inventory:
        break;
    default:
        break;
    }
    
    if (OwnerActor->Implements<UPlayerInterface>())
    {
        IPlayerInterface::Execute_UpdateItem(OwnerActor,ContainerType,ItemInfo, Index);
    } 
    

}

//===========================================FindEmptySlot====================================================
bool UItemContainerBase::FindEmptySlot(bool& Success, int32& EmptyIndex) const
{
    // Initialize with invalid state
    Success = false;
    EmptyIndex = -1;

    for (int32 Index = 0; Index <Items.Num(); ++Index)
    {
        if (Items[Index].RegistryKey.IsNone())
        {
            Success = true;
            EmptyIndex = Index;
            break;
        }
    }

    return Success;

}
//===========================================AddItem====================================================
// In ItemContainerBase.cpp
bool UItemContainerBase::AddItem(const FItemStructure& Item)
{
    if (GetOwnerRole() != ROLE_Authority)
    {
        Server_AddItem(Item);
        return true;
    }
    
    // Create a working copy of the item data
    FItemStructure LocalItemInfo = Item;
    
    // Ensure we have a valid asset reference before proceeding
    if (LocalItemInfo.ItemAsset.IsNull())
    {
        // If we have a registry key but no asset reference, try to resolve it
        if (!LocalItemInfo.RegistryKey.IsNone())
        {
            UAssetManager* AssetManager = UAssetManager::GetIfInitialized();
            if (AssetManager)
            {
                // Create a primary asset ID using the registry key
                FPrimaryAssetId ItemId = FPrimaryAssetId(FPrimaryAssetType("Item"), LocalItemInfo.RegistryKey);
                
                // Log the attempt for debugging
                UE_LOG(LogTemp, Log, TEXT("Resolving asset for %s (ID: %s)"), 
                    *LocalItemInfo.RegistryKey.ToString(), 
                    *ItemId.ToString());
                    
                // Get the path from the asset manager
                FSoftObjectPath ItemPath = AssetManager->GetPrimaryAssetPath(ItemId);
                if (!ItemPath.IsNull())
                {
                    UE_LOG(LogTemp, Log, TEXT("Successfully resolved path: %s"), *ItemPath.ToString());
                    LocalItemInfo.ItemAsset = TSoftObjectPtr<UItemInfo>(ItemPath);
                }
                else
                {
                    // Try to find the asset through direct query if path resolution failed
                    UObject* ItemObj = AssetManager->GetPrimaryAssetObject(ItemId);
                    if (ItemObj)
                    {
                        UE_LOG(LogTemp, Log, TEXT("Found asset via direct query: %s"), *ItemObj->GetPathName());
                        LocalItemInfo.ItemAsset = TSoftObjectPtr<UItemInfo>(FSoftObjectPath(ItemObj->GetPathName()));
                    }
                    else
                    {
                        // Plan B: Try to find the item by searching all UItemInfo assets
                        
                        UObjectLibrary* ItemLibrary = UObjectLibrary::CreateLibrary(UItemInfo::StaticClass(), true, true);
                        ItemLibrary->LoadAssetDataFromPath("/Game/_MAIN/Data");
                        
                        TArray<FAssetData> AllItemAssets;
                        ItemLibrary->GetAssetDataList(AllItemAssets);
                        
                        bool bFoundMatch = false;
                        for (const FAssetData& AssetData : AllItemAssets)
                        {
                            UItemInfo* PotentialItem = Cast<UItemInfo>(AssetData.GetAsset());
                            if (PotentialItem && PotentialItem->RegistryKey == LocalItemInfo.RegistryKey)
                            {
                                UE_LOG(LogTemp, Log, TEXT("Found matching item through content search: %s"), 
                                    *AssetData.GetSoftObjectPath().ToString());
                                LocalItemInfo.ItemAsset = TSoftObjectPtr<UItemInfo>(AssetData.GetSoftObjectPath());
                                bFoundMatch = true;
                                break;
                            }
                        }
                        
                        if (!bFoundMatch)
                        {
                            UE_LOG(LogTemp, Warning, TEXT("Failed to resolve asset for key: %s"), 
                                *LocalItemInfo.RegistryKey.ToString());
                            
                            // Log all available Item assets for debugging
                            TArray<FPrimaryAssetId> AllItems;
                            AssetManager->GetPrimaryAssetIdList(FPrimaryAssetType("Item"), AllItems);
                            UE_LOG(LogTemp, Log, TEXT("Available Item assets (%d):"), AllItems.Num());
                            for (const FPrimaryAssetId& AssetId : AllItems)
                            {
                                UE_LOG(LogTemp, Log, TEXT("  - %s"), *AssetId.ToString());
                            }
                            
                            UE_LOG(LogTemp, Log, TEXT("Direct content search found %d potential items:"), AllItemAssets.Num());
                            for (int32 i = 0; i < FMath::Min(10, AllItemAssets.Num()); i++)
                            {
                                UE_LOG(LogTemp, Log, TEXT("  - %s"), *AllItemAssets[i].AssetName.ToString());
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Double-check that we now have a valid asset path
    if (LocalItemInfo.ItemAsset.IsNull())
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot add item with invalid asset reference. RegKey: %s"), 
            LocalItemInfo.RegistryKey.IsNone() ? TEXT("NONE") : *LocalItemInfo.RegistryKey.ToString());
        return false;
    }

    // Find an empty slot
    bool Success;
    int32 LocalEmptyIndex;
    FindEmptySlot(Success, LocalEmptyIndex);

    if (Success)
    {
        // Log the item being added with its asset path for debugging
        UE_LOG(LogTemp, Log, TEXT("Adding item to slot %d: Key=%s, AssetPath=%s"), 
            LocalEmptyIndex,
            *LocalItemInfo.RegistryKey.ToString(),
            *LocalItemInfo.ItemAsset.ToSoftObjectPath().ToString());
            
        Items[LocalEmptyIndex] = LocalItemInfo;
        UpdateUI(LocalEmptyIndex, LocalItemInfo);
        return true;
    }

    UE_LOG(LogTemp, Warning, TEXT("No empty slot found for item: %s"), 
        *LocalItemInfo.RegistryKey.ToString());
    return false;
}