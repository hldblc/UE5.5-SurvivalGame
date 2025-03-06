#include "Data/Library/ItemAssetCache.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

// Initialize the static cache map.
TMap<FString, TWeakObjectPtr<UTexture2D>> UItemAssetCache::CachedTextures;

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
			CachedTextures.Add(Key, LoadedTexture);
		}
		// Execute the callback with the loaded texture (or nullptr if failed).
		OnLoaded.ExecuteIfBound(LoadedTexture);
	}));
}