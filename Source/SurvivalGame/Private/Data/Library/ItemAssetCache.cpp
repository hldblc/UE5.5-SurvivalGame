// ItemAssetCache.cpp
#include "Data/Library/ItemAssetCache.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

// Initialize the static cache maps.
TMap<FString, TWeakObjectPtr<UTexture2D>> UItemAssetCache::CachedTextures;
TMap<FString, TWeakObjectPtr<UItemInfo>> UItemAssetCache::CachedItemInfos;

UTexture2D* UItemAssetCache::GetCachedItemIcon(const FSoftObjectPath& AssetPath)
{
	if (!AssetPath.IsValid())
	{
		return nullptr;
	}

	FString Key = AssetPath.ToString();
	if (CachedTextures.Contains(Key) && CachedTextures[Key].IsValid())
	{
		return CachedTextures[Key].Get();
	}
    
	return nullptr;
}

void UItemAssetCache::RequestItemIconAsync(const FSoftObjectPath& AssetPath, const FOnTextureLoaded& OnLoaded)
{
	if (!AssetPath.IsValid())
	{
		OnLoaded.ExecuteIfBound(nullptr);
		return;
	}

	FString Key = AssetPath.ToString();
    
	// If already cached, immediately execute the callback.
	if (CachedTextures.Contains(Key) && CachedTextures[Key].IsValid())
	{
		OnLoaded.ExecuteIfBound(CachedTextures[Key].Get());
		return;
	}
    
	// Use the StreamableManager to load the asset asynchronously.
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	Streamable.RequestAsyncLoad(AssetPath, FStreamableDelegate::CreateLambda([AssetPath, Key, OnLoaded]()
	{
		UTexture2D* LoadedTexture = Cast<UTexture2D>(AssetPath.TryLoad());
		if (LoadedTexture)
		{
			// Cache the texture for future use.
			UItemAssetCache::CachedTextures.Add(Key, LoadedTexture);
		}
		// Execute the callback with the loaded texture (or nullptr if failed).
		OnLoaded.ExecuteIfBound(LoadedTexture);
	}));
}

UItemInfo* UItemAssetCache::GetCachedItemInfo(const FSoftObjectPath& AssetPath)
{
	if (!AssetPath.IsValid())
	{
		return nullptr;
	}

	FString Key = AssetPath.ToString();
	if (CachedItemInfos.Contains(Key) && CachedItemInfos[Key].IsValid())
	{
		return CachedItemInfos[Key].Get();
	}
    
	return nullptr;
}

void UItemAssetCache::RequestItemInfoAsync(const FSoftObjectPath& AssetPath, const FOnItemInfoLoaded& OnLoaded)
{
	if (!AssetPath.IsValid())
	{
		OnLoaded.ExecuteIfBound(nullptr);
		return;
	}

	FString Key = AssetPath.ToString();
    
	// If already cached, immediately execute the callback.
	if (CachedItemInfos.Contains(Key) && CachedItemInfos[Key].IsValid())
	{
		OnLoaded.ExecuteIfBound(CachedItemInfos[Key].Get());
		return;
	}
    
	// Use the StreamableManager to load the asset asynchronously.
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	Streamable.RequestAsyncLoad(AssetPath, FStreamableDelegate::CreateLambda([AssetPath, Key, OnLoaded]()
	{
		UItemInfo* LoadedItemInfo = Cast<UItemInfo>(AssetPath.TryLoad());
		if (LoadedItemInfo)
		{
			// Cache the item info for future use.
			UItemAssetCache::CachedItemInfos.Add(Key, LoadedItemInfo);
		}
		// Execute the callback with the loaded item info (or nullptr if failed).
		OnLoaded.ExecuteIfBound(LoadedItemInfo);
	}));
}

void UItemAssetCache::ClearCache()
{
	CachedTextures.Empty();
	CachedItemInfos.Empty();
}