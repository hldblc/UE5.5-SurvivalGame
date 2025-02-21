// SurvivalAssetManager.cpp

#include "SurvivalAssetManager.h"
#include "Engine/AssetManager.h"
#include "PrimaryData/ItemInfo.h"

void USurvivalAssetManager::StartInitialLoading()
{
	// Let the base class do its standard work first
	Super::StartInitialLoading();

	UE_LOG(LogTemp, Log, TEXT("USurvivalAssetManager: StartInitialLoading() called"));
    
	// Make sure primary asset types are properly registered
	FPrimaryAssetType ItemType("Item");
    
	// Scan for primary assets
	TArray<FString> Paths;
	Paths.Add("/Game/_MAIN/Data");
    
	
	// Format: (PrimaryAssetType, Paths, BaseClass, bHasBlueprintClasses, bIsEditorOnly, bForceSynchronousScan)
	ScanPathsForPrimaryAssets(ItemType, Paths, UItemInfo::StaticClass(), true, false, false);
    
	
	TArray<FPrimaryAssetId> ItemAssetIds;
	GetPrimaryAssetIdList(ItemType, ItemAssetIds);

	// If have items, request them to load asynchronously
	if (ItemAssetIds.Num() > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("Found %d 'Item' assets:"), ItemAssetIds.Num());
		for (int32 i = 0; i < FMath::Min(ItemAssetIds.Num(), 10); i++)
		{
			UE_LOG(LogTemp, Log, TEXT("  [%d] %s"), i, *ItemAssetIds[i].ToString());
		}
        
		if (ItemAssetIds.Num() > 10)
		{
			UE_LOG(LogTemp, Log, TEXT("  ...and %d more"), ItemAssetIds.Num() - 10);
		}

		FStreamableDelegate OnLoadCompleteDelegate = FStreamableDelegate::CreateUObject(
			this, 
			&USurvivalAssetManager::OnItemsLoaded
		);

		// This will queue up an async load of all found item assets
		LoadPrimaryAssets(ItemAssetIds, TArray<FName>(), OnLoadCompleteDelegate);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No 'Item' assets found to load! Check paths in ScanPathsForPrimaryAssets"));
	}
}

void USurvivalAssetManager::OnItemsLoaded() const
{
	UE_LOG(LogTemp, Log, TEXT("All 'Item' assets have finished loading!"));

	// At this point, the items are loaded in memory. 
	TArray<FPrimaryAssetId> LoadedItemIds;
	GetPrimaryAssetIdList(FPrimaryAssetType("Item"), LoadedItemIds);

	for (auto& AssetId : LoadedItemIds)
	{
		// The Asset Manager should now have them in memory
		UObject* LoadedObj = GetPrimaryAssetObject(AssetId);
		if (LoadedObj)
		{
			UE_LOG(LogTemp, Log, TEXT("Loaded asset: %s"), *LoadedObj->GetName());
		}
	}
}
