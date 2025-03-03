#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "ItemFactory.generated.h"

class UItemInfo;

/**
 * UItemFactory
 * 
 * Responsible for creating item assets from a given DataTable, 
 * as well as updating or mass-creating all items in that table.
 * This should be used only in the editor (WITH_EDITOR).
 */
UCLASS()
class SURVIVALGAMEEDITOR_API UItemFactory : public UObject
{
	GENERATED_BODY()

public:
	// Base path for creating new assets
	UPROPERTY(EditDefaultsOnly, Category = "Configuration")
	FString BaseContentPath = TEXT("/Game/_MAIN/Data/Items");

	// The Data Table containing all item definitions
	UPROPERTY(EditDefaultsOnly, Category = "Configuration")
	TSoftObjectPtr<UDataTable> ItemDataTable;

#if WITH_EDITOR
	/**
	 * Create a single item asset based on the specified row.
	 * @param RowName Name of the row in the DataTable
	 * @param SubPath Optional subdirectory path under BaseContentPath for the new asset
	 * @return The newly created (or updated) UItemInfo asset
	 */
	UFUNCTION(BlueprintCallable, Category = "Item Creation")
	UItemInfo* CreateItemDataAsset(FName RowName, const FString& SubPath = TEXT(""));

	/**
	 * Create all items from the referenced DataTable.
	 * @return An array of created/updated UItemInfo assets
	 */
	UFUNCTION(BlueprintCallable, Category = "Item Creation")
	TArray<UItemInfo*> CreateAllItemAssets();

	/**
	 * Update existing items that match rows in the DataTable.
	 * (e.g., reloading stats if you changed the DataTable).
	 */
	UFUNCTION(BlueprintCallable, Category = "Item Management")
	void UpdateAllItems();

private:
	/**
	 * Helper function to build a valid asset path with no extra dot/extension.
	 * e.g. /Game/_MAIN/Data/Items/SubPath/AssetName
	 */
	FString GetFullPath(const FString& SubPath, const FString& AssetName) const;
#endif
};
