#include "Tools/ItemFactory.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "EditorAssetLibrary.h"
#include "DataTables/ItemDataTableRow.h"
#include "PrimaryData/ItemInfo.h"
#include "Misc/PackageName.h"
#include "UObject/Package.h"
#include "HAL/FileManager.h"
#include "Modules/ModuleManager.h"
#include "Logging/LogMacros.h"
#include "Interfaces/IPluginManager.h"
// For SavePackage + FSavePackageArgs:
#include "PackageTools.h"  // or "PackageTools.h" in some UE versions
#include "UObject/UObjectGlobals.h"
#include "UObject/SavePackage.h"

#if WITH_EDITOR

UItemInfo* UItemFactory::CreateItemDataAsset(FName RowName, const FString& SubPath /*= TEXT("")*/)
{
    // 1) Make sure the DataTable is valid
    if (!ItemDataTable.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("ItemFactory: No Data Table set!"));
        return nullptr;
    }

    UDataTable* Table = ItemDataTable.LoadSynchronous();
    if (!Table)
    {
        UE_LOG(LogTemp, Error, TEXT("ItemFactory: Failed to load Data Table!"));
        return nullptr;
    }

    // 2) Find the row in the table
    FItemDataTableRow* RowData = Table->FindRow<FItemDataTableRow>(RowName, TEXT(""));
    if (!RowData)
    {
        UE_LOG(LogTemp, Error, TEXT("ItemFactory: Row '%s' not found in Data Table!"), *RowName.ToString());
        return nullptr;
    }

    // 3) Build a valid package path with NO dot extension
    //    e.g. "/Game/_MAIN/Data/Items/SubPath/DA_WoodenLog"
    FString AssetName = FString::Printf(TEXT("DA_%s"), *RowName.ToString());
    FString PackagePath = GetFullPath(SubPath, AssetName);

    // 4) Create or find the package
    UPackage* Package = CreatePackage(*PackagePath);
    if (!Package)
    {
        UE_LOG(LogTemp, Error, TEXT("ItemFactory: Failed to create/find package for '%s'"), *PackagePath);
        return nullptr;
    }

    // 5) Check if there's already a UItemInfo in that package
    UItemInfo* NewItem = FindObject<UItemInfo>(Package, *AssetName);
    if (!NewItem)
    {
        // Create a new UItemInfo
        NewItem = NewObject<UItemInfo>(Package, UItemInfo::StaticClass(), *AssetName, RF_Public | RF_Standalone);
        if (!NewItem)
        {
            UE_LOG(LogTemp, Error, TEXT("ItemFactory: Failed to create '%s'"), *AssetName);
            return nullptr;
        }
        // Inform the asset registry that we've created a new asset
        FAssetRegistryModule::AssetCreated(NewItem);
    }

    // 6) Update this item asset with data from the row
    NewItem->DataTableRowName = RowName;
    NewItem->LoadFromDataTable(Table);

    // Mark it dirty so the Editor knows it's modified
    NewItem->MarkPackageDirty();

    // 7) Save the package using FSavePackageArgs
    FString FilePath = FPackageName::LongPackageNameToFilename(
        PackagePath,
        FPackageName::GetAssetPackageExtension()
    );

    FSavePackageArgs SaveArgs;
    SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
    // SaveArgs.Error = GError; // or your own error device
    // SaveArgs.SaveFlags = SAVE_NoError;
    // etc.

    const bool bSaved = UPackage::SavePackage(
        Package,
        NewItem,
        *FilePath,
        SaveArgs
    );

    if (!bSaved)
    {
        UE_LOG(LogTemp, Error, TEXT("ItemFactory: Failed to save package '%s'"), *FilePath);
        return nullptr;
    }

    UE_LOG(LogTemp, Log, TEXT("ItemFactory: Successfully created/updated asset '%s'"), *AssetName);
    return NewItem;
}

TArray<UItemInfo*> UItemFactory::CreateAllItemAssets()
{
    TArray<UItemInfo*> CreatedAssets;
    if (!ItemDataTable.IsValid())
    {
        return CreatedAssets;
    }

    UDataTable* Table = ItemDataTable.LoadSynchronous();
    if (!Table)
    {
        return CreatedAssets;
    }

    // Get all row names
    TArray<FName> RowNames = Table->GetRowNames();

    // Create or update assets for each row
    for (const FName& RowName : RowNames)
    {
        if (UItemInfo* Asset = CreateItemDataAsset(RowName))
        {
            CreatedAssets.Add(Asset);
        }
    }
    return CreatedAssets;
}

void UItemFactory::UpdateAllItems()
{
    if (!ItemDataTable.IsValid())
    {
        return;
    }

    UDataTable* Table = ItemDataTable.LoadSynchronous();
    if (!Table)
    {
        return;
    }

    // Find all existing UItemInfo assets in project
    TArray<FAssetData> ExistingAssets;
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

    FARFilter Filter;
    Filter.ClassPaths.Add(UItemInfo::StaticClass()->GetClassPathName());
    AssetRegistryModule.Get().GetAssets(Filter, ExistingAssets);

    // Reload data from the table for each asset
    for (const FAssetData& AssetData : ExistingAssets)
    {
        if (UItemInfo* Item = Cast<UItemInfo>(AssetData.GetAsset()))
        {
            if (!Item->DataTableRowName.IsNone())
            {
                Item->LoadFromDataTable(Table);
                Item->MarkPackageDirty();
            }
        }
    }
}

FString UItemFactory::GetFullPath(const FString& SubPath, const FString& AssetName) const
{
    // e.g. BaseContentPath = "/Game/_MAIN/Data/Items"
    // Combine it with any subfolder, then the final asset name
    FString FullPath = BaseContentPath;

    if (!SubPath.IsEmpty())
    {
        FullPath = FPaths::Combine(FullPath, SubPath);
    }

    return FPaths::Combine(FullPath, AssetName);
}

#endif // WITH_EDITOR
