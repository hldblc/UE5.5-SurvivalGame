// Add this to your ItemFactoryTool.h

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "Engine/DataTable.h"
#include "ItemFactory.h"
#include "Components/ComboBoxString.h" 
#include "ItemFactoryTool.generated.h"

/**
 * Editor Utility Widget for creating Item assets from a DataTable
 */
UCLASS()
class SURVIVALGAMEEDITOR_API UItemFactoryTool : public UEditorUtilityWidget
{
    GENERATED_BODY()

public:
    /** The data table containing item definitions we want to create assets for */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Configuration", meta=(DisplayName="Item Data Table"))
    UDataTable* ItemDataTable;
    
    /** Reference to the folder selector combo box */
    UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
    UComboBoxString* Combo_FolderSelector;
    
    /** Reference to the item selector combo box */
    UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
    UComboBoxString* Combo_SelectItem;

    /**
     * Creates new assets for *all* rows in the data table
     * and returns an array of the UItemInfo objects that were created.
     */
    UFUNCTION(BlueprintCallable, Category = "Item Creation")
    TArray<UItemInfo*> CreateAllItems()
    {
       UItemFactory* Factory = NewObject<UItemFactory>();
       Factory->ItemDataTable = ItemDataTable;
       return Factory->CreateAllItemAssets();
    }

    /**
     * Creates a single item asset for the specified row name
     * using the folder path selected in the dropdown.
     */
    UFUNCTION(BlueprintCallable, Category = "Item Creation")
    UItemInfo* CreateSingleItemWithSelectedPath(FName RowName)
    {
        if (!Combo_FolderSelector)
        {
            return nullptr;
        }
    
        // Get the selected folder path from the combo box
        FString SelectedFolderPath = Combo_FolderSelector->GetSelectedOption();
    
        UItemFactory* Factory = NewObject<UItemFactory>();
        Factory->ItemDataTable = ItemDataTable;
    
        // If it's a full path, override the base path
        if (SelectedFolderPath.StartsWith(TEXT("/Game/")))
        {
            // Extract just the directory part without any filename
            FString Directory = FPaths::GetPath(SelectedFolderPath);
            Factory->BaseContentPath = Directory;
            return Factory->CreateItemDataAsset(RowName);
        }
        else
        {
            // Use as subpath
            return Factory->CreateItemDataAsset(RowName, SelectedFolderPath);
        }
    }
};