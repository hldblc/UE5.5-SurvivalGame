#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Data/DataTables/ItemDataTableRow.h" 
#include "ItemDataAccessLibrary.generated.h"

/**
 * Utility library for accessing and manipulating item data from data tables
 */
UCLASS(Blueprintable, BlueprintType, meta = (DisplayName = "Item Data Access Library"))
class SURVIVALGAME_API UItemDataAccessLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
    
public:
    /**
     * Retrieves an item's icon texture from a data table
     * @param DataTable The data table containing the item data
     * @param RowName The row name to look up
     * @return The item's icon texture or nullptr if not found
     */
    UFUNCTION(BlueprintPure, Category = "Item Data|Properties")
    static UTexture2D* GetItemIcon(const UDataTable* DataTable, FName RowName);
    
    /**
     * Asynchronously loads an item's icon texture from a data table
     * @param WorldContextObject World context
     * @param DataTable The data table containing the item data
     * @param RowName The row name to look up
     * @param OutTexture The loaded texture (output parameter)
     * @param LatentInfo Latent action information
     */
    UFUNCTION(BlueprintCallable, Category = "Item Data|Properties", meta = (Latent, LatentInfo = "LatentInfo", WorldContext = "WorldContextObject"))
    static void GetItemIconAsync(UObject* WorldContextObject, const UDataTable* DataTable, FName RowName, UTexture2D*& OutTexture, FLatentActionInfo LatentInfo);
    
    /**
     * Gets an item's display name
     */
    UFUNCTION(BlueprintPure, Category = "Item Data|Properties")
    static FText GetItemName(const UDataTable* DataTable, FName RowName);
    
    /**
     * Gets an item's description text
     */
    UFUNCTION(BlueprintPure, Category = "Item Data|Properties")
    static FText GetItemDescription(const UDataTable* DataTable, FName RowName);
    
    /**
     * Gets an item's category
     */
    UFUNCTION(BlueprintPure, Category = "Item Data|Properties")
    static E_ItemCategory GetItemCategory(const UDataTable* DataTable, FName RowName);
    
    /**
     * Gets an item's type
     */
    UFUNCTION(BlueprintPure, Category = "Item Data|Properties")
    static E_ItemType GetItemType(const UDataTable* DataTable, FName RowName);
    
    /**
     * Gets complete row data for an item
     */
    UFUNCTION(BlueprintCallable, Category = "Item Data|Row Access")
    static bool GetItemRowData(const UDataTable* DataTable, FName RowName, FItemDataTableRow& OutRowData);
    
    /**
     * Checks if an item is stackable
     */
    UFUNCTION(BlueprintPure, Category = "Item Data|Attributes")
    static bool IsItemStackable(const UDataTable* DataTable, FName RowName);
    
    /**
     * Checks if an item is consumable
     */
    UFUNCTION(BlueprintPure, Category = "Item Data|Attributes")
    static bool IsItemConsumable(const UDataTable* DataTable, FName RowName);
    
    /**
     * Checks if an item is a weapon
     */
    UFUNCTION(BlueprintPure, Category = "Item Data|Attributes")
    static bool IsItemWeapon(const UDataTable* DataTable, FName RowName);
    
    /**
     * Checks if an item is a resource
     */
    UFUNCTION(BlueprintPure, Category = "Item Data|Attributes")
    static bool IsItemResource(const UDataTable* DataTable, FName RowName);
    
    /**
     * Gets an item's maximum stack size
     */
    UFUNCTION(BlueprintPure, Category = "Item Data|Properties")
    static int32 GetItemMaxStackSize(const UDataTable* DataTable, FName RowName);
    
    /**
     * Gets an item's weight
     */
    UFUNCTION(BlueprintPure, Category = "Item Data|Properties")
    static float GetItemWeight(const UDataTable* DataTable, FName RowName);
};