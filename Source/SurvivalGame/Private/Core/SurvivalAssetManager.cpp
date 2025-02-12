// SurvivalAssetManager.cpp

#include "SurvivalAssetManager.h"
#include "Engine/AssetManager.h"

void USurvivalAssetManager::StartInitialLoading()
{
	// Let the base class do its standard work first
	Super::StartInitialLoading();

	UE_LOG(LogTemp, Log, TEXT("USurvivalAssetManager: StartInitialLoading() called"));

	// Grab all items of type "Item"
	TArray<FPrimaryAssetId> ItemAssetIds;
	GetPrimaryAssetIdList(FPrimaryAssetType("Item"), ItemAssetIds);

	// If you have items, request them to load asynchronously
	if (ItemAssetIds.Num() > 0)
	{
		FStreamableDelegate OnLoadCompleteDelegate = FStreamableDelegate::CreateUObject(
			this, 
			&USurvivalAssetManager::OnItemsLoaded
		);

		// This will queue up an async load of all found item assets
		LoadPrimaryAssets(ItemAssetIds, TArray<FName>(), OnLoadCompleteDelegate);

		UE_LOG(LogTemp, Log, TEXT("Requested async load for %d 'Item' assets."), ItemAssetIds.Num());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No 'Item' assets found to load."));
	}
}

void USurvivalAssetManager::OnItemsLoaded()
{
	UE_LOG(LogTemp, Log, TEXT("All 'Item' assets have finished loading!"));

	// At this point, the items are loaded in memory. If you want references:
	TArray<FPrimaryAssetId> LoadedItemIds;
	GetPrimaryAssetIdList(FPrimaryAssetType("Item"), LoadedItemIds);

	for (auto& AssetId : LoadedItemIds)
	{
		// The Asset Manager should now have them in memory
		UObject* LoadedObj = GetPrimaryAssetObject(AssetId);
		if (LoadedObj)
		{
			UE_LOG(LogTemp, Log, TEXT("Loaded asset: %s"), *LoadedObj->GetName());

			// If it's a UItemInfo, you can cast and store it somewhere
			// e.g., keep a TMap<FName, UItemInfo*> if you want quick access
		}
	}
}
