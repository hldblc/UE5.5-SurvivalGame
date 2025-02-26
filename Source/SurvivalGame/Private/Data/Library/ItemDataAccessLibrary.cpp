#include "Data/Library/ItemDataAccessLibrary.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

UTexture2D* UItemDataAccessLibrary::GetItemIcon(const UDataTable* DataTable, FName RowName)
{
    if (!DataTable || RowName.IsNone())
    {
        return nullptr;
    }
    
    static const FString ContextString(TEXT("GetItemIcon"));
    FItemDataTableRow* RowData = DataTable->FindRow<FItemDataTableRow>(RowName, ContextString);
    
    if (!RowData || RowData->ItemIcon.IsNull())
    {
        return nullptr;
    }
    
    return RowData->ItemIcon.LoadSynchronous();
}

void UItemDataAccessLibrary::GetItemIconAsync(UObject* WorldContextObject, const UDataTable* DataTable, FName RowName, UTexture2D*& OutTexture, FLatentActionInfo LatentInfo)
{
    OutTexture = nullptr;
    
    if (!WorldContextObject || !DataTable || RowName.IsNone())
    {
        return;
    }
    
    static const FString ContextString(TEXT("GetItemIconAsync"));
    FItemDataTableRow* RowData = DataTable->FindRow<FItemDataTableRow>(RowName, ContextString);
    
    if (!RowData || RowData->ItemIcon.IsNull())
    {
        return;
    }
    
    // Create a weak reference to track if the world context object is destroyed
    TWeakObjectPtr<UObject> WeakWorldContextObject(WorldContextObject);
    
    // Start async load
    FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
    StreamableManager.RequestAsyncLoad(
        RowData->ItemIcon.ToSoftObjectPath(),
        FStreamableDelegate::CreateLambda([WeakWorldContextObject, &OutTexture, RowData, LatentInfo]() {
            // Check if the context is still valid
            if (WeakWorldContextObject.IsValid())
            {
                OutTexture = RowData->ItemIcon.Get();
                
                // Complete the latent action
                if (UWorld* World = GEngine->GetWorldFromContextObject(WeakWorldContextObject.Get(), EGetWorldErrorMode::LogAndReturnNull))
                {
                    FLatentActionManager& LatentActionManager = World->GetLatentActionManager();
                    // This is the line that needs to be fixed - RemoveActionsForObject only takes one parameter
                    LatentActionManager.RemoveActionsForObject(WeakWorldContextObject.Get());
                }
            }
        })
    );
}

FText UItemDataAccessLibrary::GetItemName(const UDataTable* DataTable, FName RowName)
{
    if (!DataTable || RowName.IsNone())
    {
        return FText::GetEmpty();
    }
    
    static const FString ContextString(TEXT("GetItemName"));
    FItemDataTableRow* RowData = DataTable->FindRow<FItemDataTableRow>(RowName, ContextString);
    
    if (!RowData)
    {
        return FText::GetEmpty();
    }
    
    return RowData->ItemName;
}

FText UItemDataAccessLibrary::GetItemDescription(const UDataTable* DataTable, FName RowName)
{
    if (!DataTable || RowName.IsNone())
    {
        return FText::GetEmpty();
    }
    
    static const FString ContextString(TEXT("GetItemDescription"));
    FItemDataTableRow* RowData = DataTable->FindRow<FItemDataTableRow>(RowName, ContextString);
    
    if (!RowData)
    {
        return FText::GetEmpty();
    }
    
    return RowData->ItemDescription;
}

E_ItemCategory UItemDataAccessLibrary::GetItemCategory(const UDataTable* DataTable, FName RowName)
{
    if (!DataTable || RowName.IsNone())
    {
        return E_ItemCategory::None;
    }
    
    static const FString ContextString(TEXT("GetItemCategory"));
    FItemDataTableRow* RowData = DataTable->FindRow<FItemDataTableRow>(RowName, ContextString);
    
    if (!RowData)
    {
        return E_ItemCategory::None;
    }
    
    return RowData->ItemCategory;
}

E_ItemType UItemDataAccessLibrary::GetItemType(const UDataTable* DataTable, FName RowName)
{
    if (!DataTable || RowName.IsNone())
    {
        return E_ItemType::None;
    }
    
    static const FString ContextString(TEXT("GetItemType"));
    FItemDataTableRow* RowData = DataTable->FindRow<FItemDataTableRow>(RowName, ContextString);
    
    if (!RowData)
    {
        return E_ItemType::None;
    }
    
    return RowData->ItemType;
}

bool UItemDataAccessLibrary::GetItemRowData(const UDataTable* DataTable, FName RowName, FItemDataTableRow& OutRowData)
{
    if (!DataTable || RowName.IsNone())
    {
        return false;
    }
    
    static const FString ContextString(TEXT("GetItemRowData"));
    FItemDataTableRow* RowData = DataTable->FindRow<FItemDataTableRow>(RowName, ContextString);
    
    if (!RowData)
    {
        return false;
    }
    
    OutRowData = *RowData;
    return true;
}

bool UItemDataAccessLibrary::IsItemStackable(const UDataTable* DataTable, FName RowName)
{
    if (!DataTable || RowName.IsNone())
    {
        return false;
    }
    
    static const FString ContextString(TEXT("IsItemStackable"));
    FItemDataTableRow* RowData = DataTable->FindRow<FItemDataTableRow>(RowName, ContextString);
    
    if (!RowData)
    {
        return false;
    }
    
    return RowData->bStackable;
}

bool UItemDataAccessLibrary::IsItemConsumable(const UDataTable* DataTable, FName RowName)
{
    if (!DataTable || RowName.IsNone())
    {
        return false;
    }
    
    static const FString ContextString(TEXT("IsItemConsumable"));
    FItemDataTableRow* RowData = DataTable->FindRow<FItemDataTableRow>(RowName, ContextString);
    
    if (!RowData)
    {
        return false;
    }
    
    return (RowData->ItemCategory == E_ItemCategory::Consumable) || 
           (RowData->ItemType == E_ItemType::Food) || 
           (RowData->ItemType == E_ItemType::Potion);
}

bool UItemDataAccessLibrary::IsItemWeapon(const UDataTable* DataTable, FName RowName)
{
    if (!DataTable || RowName.IsNone())
    {
        return false;
    }
    
    static const FString ContextString(TEXT("IsItemWeapon"));
    FItemDataTableRow* RowData = DataTable->FindRow<FItemDataTableRow>(RowName, ContextString);
    
    if (!RowData)
    {
        return false;
    }
    
    return RowData->ItemCategory == E_ItemCategory::Equipment;
}

bool UItemDataAccessLibrary::IsItemResource(const UDataTable* DataTable, FName RowName)
{
    if (!DataTable || RowName.IsNone())
    {
        return false;
    }
    
    static const FString ContextString(TEXT("IsItemResource"));
    FItemDataTableRow* RowData = DataTable->FindRow<FItemDataTableRow>(RowName, ContextString);
    
    if (!RowData)
    {
        return false;
    }
    
    return RowData->ItemCategory == E_ItemCategory::Resource;
}

int32 UItemDataAccessLibrary::GetItemMaxStackSize(const UDataTable* DataTable, FName RowName)
{
    if (!DataTable || RowName.IsNone())
    {
        return 1;
    }
    
    static const FString ContextString(TEXT("GetItemMaxStackSize"));
    FItemDataTableRow* RowData = DataTable->FindRow<FItemDataTableRow>(RowName, ContextString);
    
    if (!RowData)
    {
        return 1;
    }
    
    return RowData->bStackable ? RowData->StackSize : 1;
}

float UItemDataAccessLibrary::GetItemWeight(const UDataTable* DataTable, FName RowName)
{
    if (!DataTable || RowName.IsNone())
    {
        return 0.0f;
    }
    
    static const FString ContextString(TEXT("GetItemWeight"));
    FItemDataTableRow* RowData = DataTable->FindRow<FItemDataTableRow>(RowName, ContextString);
    
    if (!RowData)
    {
        return 0.0f;
    }
    
    return RowData->ItemWeight;
}