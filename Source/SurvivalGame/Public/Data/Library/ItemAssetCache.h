#pragma once

#include "CoreMinimal.h"
#include "Delegates/Delegate.h"  
#include "Engine/Texture2D.h"
#include "UObject/NoExportTypes.h"
#include "ItemAssetCache.generated.h"

// Non-dynamic delegate for pure C++ usage (supports lambda binding)
DECLARE_DELEGATE_OneParam(FOnTextureLoaded, UTexture2D*);

/**
 * Utility class for caching item assets (such as textures) to reduce load times.
 */
UCLASS()
class SURVIVALGAME_API UItemAssetCache : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Returns a cached texture if available.
	 *
	 * @param AssetPath The soft object path to the texture.
	 * @return The cached UTexture2D pointer if it exists; otherwise, nullptr.
	 */
	UFUNCTION(BlueprintCallable, Category = "Asset Cache")
	static UTexture2D* GetCachedItemIcon(const FSoftObjectPath& AssetPath);

	/**
	 * Asynchronously loads the texture if it's not in the cache.
	 *
	 * @param AssetPath The soft object path to the texture.
	 * @param OnLoaded Callback invoked when the texture is loaded.
	 */
	static void RequestItemIconAsync(const FSoftObjectPath& AssetPath, const FOnTextureLoaded& OnLoaded);

private:
	// A static map that caches textures using the asset path as the key.
	static TMap<FString, TWeakObjectPtr<UTexture2D>> CachedTextures;
};