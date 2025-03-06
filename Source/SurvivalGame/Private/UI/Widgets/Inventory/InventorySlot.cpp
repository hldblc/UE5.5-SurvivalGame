#include "UI/Widgets/Inventory/InventorySlot.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Framework/Application/SlateApplication.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Data/PrimaryData/ItemInfo.h"  
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "Delegates/DelegateSignatureImpl.inl"
#include "Internationalization/Text.h"
#include "Library/ItemAssetCache.h"
#include "UI/Widgets/Inventory/DraggedItem.h"
#include "UI/Widgets/Inventory/Operations/ItemDrag.h"


void UInventorySlot::NativePreConstruct()
{
    Super::NativePreConstruct();
    
    // Validate widget bindings with specific errors
    if (!ItemWeight)
    {
        UE_LOG(LogTemp, Error, TEXT("InventorySlot: ItemWeight binding is missing!"));
    }
    if (!ItemIcon)
    {
        UE_LOG(LogTemp, Error, TEXT("InventorySlot: ItemIcon binding is missing!"));
    }
    if (!ItemQuantity)
    {
        UE_LOG(LogTemp, Error, TEXT("InventorySlot: ItemQuantity binding is missing!"));
    }
    if (!TopText)
    {
        UE_LOG(LogTemp, Error, TEXT("InventorySlot: TopText binding is missing!"));
    }
    if (!BottomTextAmmo)
    {
        UE_LOG(LogTemp, Error, TEXT("InventorySlot: BottomTextAmmo binding is missing!"));
    }
    if (!ItemHPBar)
    {
        UE_LOG(LogTemp, Error, TEXT("InventorySlot: ItemHPBar binding is missing!"));
    }
}

void UInventorySlot::NativeConstruct()
{
    Super::NativeConstruct();
    
    // Set initial visibility
    if (TopText)
    {
        TopText->SetVisibility(ESlateVisibility::Visible);
        TopText->SetText(FText::FromString(TEXT(""))); // Empty slot text
    }
}

FReply UInventorySlot::NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        // 2) If so, replicate the Blueprint’s "SetDragInput Pressed" path by returning Handled.
        return FReply::Handled();
    }
    return Super::NativeOnMouseButtonDown(MyGeometry, MouseEvent);
}

FReply UInventorySlot::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    // Check if the left mouse button is pressed
    if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
    {
        // Detect drag if left mouse button is pressed
        FEventReply Reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
        return Reply.NativeReply;
    }
    else
    {
        // If not left mouse button, mark as unhandled
        return FReply::Unhandled();
    }
}

void UInventorySlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
    // Only proceed if an item exists in the slot
    if (!bHasItemInSlot || !ItemAssetInfo)
    {
        return;
    }
    
    // Create the DraggedItem widget
    UDraggedItem* DragVisual = CreateWidget<UDraggedItem>(GetOwningPlayer(), DraggedItemClass);
    if (!DragVisual)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create DraggedItem widget"));
        return;
    }
    
    // Get the soft object path for the icon texture
    FSoftObjectPath IconPath = ItemAssetInfo->ItemIcon.ToSoftObjectPath();
    
    // Try to get the cached texture first
    UTexture2D* CachedIcon = UItemAssetCache::GetCachedItemIcon(IconPath);
    if (CachedIcon)
    {
        DragVisual->ImageIcon = CachedIcon;
        // Immediately update the visuals since the icon is available
        DragVisual->UpdateVisuals();
    }
    else
    {
        // Otherwise, request the texture asynchronously
        UItemAssetCache::RequestItemIconAsync(IconPath, FOnTextureLoaded::CreateLambda([DragVisual](UTexture2D* LoadedIcon)
        {
            if (LoadedIcon && DragVisual)
            {
                DragVisual->ImageIcon = LoadedIcon;
                DragVisual->UpdateVisuals();
            }
        }));
    }
    
    // Set additional properties on the DragVisual
    DragVisual->TextTop = FText::AsNumber(ItemAssetInfo->ItemDamage);
    DragVisual->ItemCategory = ItemAssetInfo->ItemCategory;
    DragVisual->Quantity = FText::AsNumber(StoredItemInfo.ItemQuantity);
    DragVisual->bUseAmmo = ItemAssetInfo->bUseAmmo;
    DragVisual->CurrentAmmo = StoredItemInfo.CurrentAmmo;
    DragVisual->MaxAmmo = StoredItemInfo.MaxAmmo;
    DragVisual->CurrentHP = StoredItemInfo.CurrentHP;
    DragVisual->MaxHP = StoredItemInfo.MaxHP;
    float TotalWeight = StoredItemInfo.ItemQuantity * ItemAssetInfo->ItemWeight;
    DragVisual->Weight = FText::AsNumber(TotalWeight);
    
    // Create the drag operation and assign the drag visual
    UItemDrag* DragOperation = NewObject<UItemDrag>();
    if (!DragOperation)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create ItemDrag operation"));
        return;
    }
    
    DragOperation->DefaultDragVisual = DragVisual;
    DragOperation->SlotIndex = ItemIndex;
    DragOperation->FromContainer = ContainerType;
    DragOperation->ItemCategory = ItemAssetInfo->ItemCategory;
    DragOperation->ArmorType = ItemAssetInfo->ArmorType;
    DragOperation->Pivot = EDragPivot::MouseDown;
    
    OutOperation = DragOperation;
}



UInventorySlot::UInventorySlot()
{
    
}

void UInventorySlot::UpdateSlot(FItemStructure ItemInfo)
{
    UE_LOG(LogTemp, Log, TEXT("UpdateSlot: Starting for slot %d"), ItemIndex);
    
    // Step 1: Mark this slot as occupied—empty slots are so last season.
    bHasItemInSlot = true;
    // Save the incoming item info for later UI updates.
    StoredItemInfo = ItemInfo;
    
    // Step 2: Build a soft pointer from the item's soft asset reference using the new API.
    TSoftObjectPtr<UPrimaryDataAsset> SoftItemAsset(ItemInfo.ItemAsset.ToSoftObjectPath());
    
    // Log the asset path to verify it's correct
    UE_LOG(LogTemp, Log, TEXT("UpdateSlot: Attempting to load asset at path: %s"), 
           *ItemInfo.ItemAsset.ToSoftObjectPath().ToString());
    
    // Step 3: If the asset is already loaded, grab it right away.
    if (SoftItemAsset.IsValid())
    {
        // Cast to our desired type (UItemInfo) if possible.
        ItemAssetInfo = Cast<UItemInfo>(SoftItemAsset.Get());
        if (ItemAssetInfo)
        {
            UE_LOG(LogTemp, Log, TEXT("UpdateSlot: Asset loaded successfully. Name: %s"), *ItemAssetInfo->GetName());
            // Update the UI elements with the new item details.
            UpdateUIElements();
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("UpdateSlot: Failed to cast asset to UItemInfo"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("UpdateSlot: Asset not loaded. Initiating async load..."));
        // Step 4: Asset isn't loaded? Time to request an async load.
        FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
        Streamable.RequestAsyncLoad(
            SoftItemAsset.ToSoftObjectPath(),
            FStreamableDelegate::CreateUObject(this, &UInventorySlot::OnItemAssetLoaded)
        );
    }
}

void UInventorySlot::OnItemAssetLoaded()
{
    if (StoredItemInfo.ItemAsset.IsNull() && !StoredItemInfo.RegistryKey.IsNone())
    {
        // Try to resolve the asset using the registry key if the path is still missing
        UAssetManager* AssetManager = UAssetManager::GetIfInitialized();
        if (AssetManager)
        {
            FPrimaryAssetId ItemId = FPrimaryAssetId(FPrimaryAssetType("Item"), StoredItemInfo.RegistryKey);
            FSoftObjectPath ItemPath = AssetManager->GetPrimaryAssetPath(ItemId);
            if (!ItemPath.IsNull())
            {
                UE_LOG(LogTemp, Log, TEXT("OnItemAssetLoaded: Fixed missing asset reference for slot %d"), ItemIndex);
                StoredItemInfo.ItemAsset = TSoftObjectPtr<UItemInfo>(ItemPath);
            }
        }
    }

    // Rebuild the soft pointer from our stored info (after async load)
    if (!StoredItemInfo.ItemAsset.IsNull())
    {
        TSoftObjectPtr<UPrimaryDataAsset> SoftItemAsset(StoredItemInfo.ItemAsset.ToSoftObjectPath());
        if (SoftItemAsset.IsValid())
        {
            // Cast to UItemInfo
            ItemAssetInfo = Cast<UItemInfo>(SoftItemAsset.Get());
            if (ItemAssetInfo)
            {
                UE_LOG(LogTemp, Log, TEXT("OnItemAssetLoaded: Slot %d loaded item %s successfully"), 
                    ItemIndex, *ItemAssetInfo->GetName());
                // Now update the UI elements
                UpdateUIElements();
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("OnItemAssetLoaded: Failed to cast to UItemInfo for slot %d"), ItemIndex);
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("OnItemAssetLoaded: Asset still not valid after load attempt for slot %d"), ItemIndex);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("OnItemAssetLoaded: No asset path available for slot %d"), ItemIndex);
    }
}

/**
 * Updates the UI elements of the inventory slot based on the item data, including visibility and data formatting.
 *
 * This method conditions the UI components such as icons, quantity labels, and category-specific details
 * (e.g., durability for equipment, stack size for resources, etc.) based on the current inventory state.
 *
 * Functional Details:
 * - Validates whether the associated item data (ItemAssetInfo) is loaded before proceeding.
 * - Adjusts visibility and sets formatted content for various UI components as follows:
 *   - **ItemWeight**: Displays the total weight (quantity * weight per item) if relevant.
 *   - **ItemIcon**: Loads and renders the item icon associated with the item data if available.
 *   - **ItemQuantity**: Updates the quantity display based on the item category and stackability.
 *   - **ItemHPBar**: Displays the durability bar for equipment and wearables with condition tracking.
 *   - **BottomTextAmmo**: Shows remaining ammunition if applicable to the item's type (e.g., weapons).
 *   - **TopText**: Hides or adjusts the top text, ensuring it's not displayed by default.
 *
 * Specific Category Adjustments:
 * - **Resource Items**: Shows quantity formatted as "xN" and hides other non-relevant components.
 * - **Equipment**: Displays durability (HP percent) and conditionally loads ammo if applicable.
 * - **Consumable Items**: Displays quantity like resources but hides any equipment-specific elements.
 * - **Wearables**: Similar to equipment, updates durability bars but does not display ammo or quantity.
 * - **Miscellaneous/Quest Items**: Evaluates stackability to determine whether to show the quantity display.
 *
 * Logging:
 * - Logs errors and success messages (e.g., loading icons or detecting null item data) for debugging purposes.
 *
 * Note:
 * - This method is written for an Unreal Engine environment and uses Slate UI components and structures.
 * - Elements like `ItemQuantity`, `ItemWeight`, `ItemIcon`, `ItemHPBar` are expected to be UMG widgets
 *   (or objects that support similar APIs for setting visibility, properties, etc.).
 */
void UInventorySlot::UpdateUIElements() const
{
    // If our asset still isn't loaded, bail out.
    if (!ItemAssetInfo)
    {
        UE_LOG(LogTemp, Warning, TEXT("UpdateUIElements: ItemAssetInfo is null. Aborting UI update."));
        return;
    }

    // --- Update ItemWeight ---
    if (ItemWeight)
    {
        float TotalWeight = ItemAssetInfo->ItemWeight * StoredItemInfo.ItemQuantity;
        FNumberFormattingOptions FormatOptions;
        FormatOptions.MinimumFractionalDigits = 1;
        FormatOptions.MaximumFractionalDigits = 1;
        FText WeightTextValue = FText::AsNumber(TotalWeight, &FormatOptions);
        ItemWeight->SetText(WeightTextValue);
        ItemWeight->SetVisibility(ESlateVisibility::Visible);
    }

    // --- Update ItemIcon ---
    if (ItemIcon)
    {
        if (ItemAssetInfo)
        {
            UE_LOG(LogTemp, Log, TEXT("UpdateUIElements: Item Asset Name: %s"), *ItemAssetInfo->GetName());
        
            // Load the icon texture
            UTexture2D* IconTexture = ItemAssetInfo->ItemIcon.LoadSynchronous();
            if (IconTexture)
            {
                FSlateBrush Brush;
                Brush.SetResourceObject(IconTexture);
                Brush.DrawAs = ESlateBrushDrawType::Image;
                Brush.Tiling = ESlateBrushTileType::NoTile;
            
                ItemIcon->SetBrush(Brush);
                ItemIcon->SetVisibility(ESlateVisibility::Visible);
            
                UE_LOG(LogTemp, Log, TEXT("UpdateUIElements: Icon texture loaded successfully"));
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("UpdateUIElements: Failed to load icon texture"));
                ItemIcon->SetVisibility(ESlateVisibility::Hidden);
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("UpdateUIElements: ItemAssetInfo is null"));
            ItemIcon->SetVisibility(ESlateVisibility::Hidden);
        }
    }
    
    // ================================= Resource =================================
    if (ItemAssetInfo->ItemCategory == E_ItemCategory::Resource)
    {
        // Update the quantity text (formatted as "x{Quantity}")
        if (ItemQuantity)
        {
            int32 Quantity = StoredItemInfo.ItemQuantity;
            FText QuantityTextValue = FText::Format(FText::FromString(TEXT("x{0}")), FText::AsNumber(Quantity));
            ItemQuantity->SetText(QuantityTextValue);
            ItemQuantity->SetVisibility(ESlateVisibility::Visible);
        }
    }
    else
    {
        // For non-resource items, hide the quantity text (for now)
        if (ItemQuantity)
        {
            ItemQuantity->SetVisibility(ESlateVisibility::Hidden);
        }
    }
    
    // --- Update TopText ---
    if (TopText)
    {
        TopText->SetVisibility(ESlateVisibility::Hidden);
    }
    
    // --- Hide BottomTextAmmo and ItemHPBar ---
    if (BottomTextAmmo)
    {
        BottomTextAmmo->SetVisibility(ESlateVisibility::Hidden);
    }
    if (ItemHPBar)
    {
        ItemHPBar->SetVisibility(ESlateVisibility::Hidden);
    }

    // ================================= Equipment =================================
    else if (ItemAssetInfo->ItemCategory == E_ItemCategory::Equipment)
    {
        // Equipment items might have HP/durability and possibly ammo
        
        // Hide quantity for equipment
        if (ItemQuantity)
        {
            ItemQuantity->SetVisibility(ESlateVisibility::Hidden);
        }
        
        // Show durability bar if HP values are valid
        if (ItemHPBar)
        {
            if (StoredItemInfo.MaxHP > 0.0f)
            {
                float HPPercent = FMath::Clamp(StoredItemInfo.CurrentHP / StoredItemInfo.MaxHP, 0.0f, 1.0f);
                ItemHPBar->SetPercent(HPPercent);
                
                // Set color based on durability percentage
                FLinearColor BarColor;
                if (HPPercent > 0.66f)
                {
                    // Green for good condition (>66%)
                    BarColor = FLinearColor(0.0f, 0.75f, 0.0f);
                }
                else if (HPPercent > 0.33f)
                {
                    // Yellow for moderate condition (33-66%)
                    BarColor = FLinearColor(0.75f, 0.75f, 0.0f);
                }
                else
                {
                    // Red for poor condition (<33%)
                    BarColor = FLinearColor(0.75f, 0.0f, 0.0f);
                }
                
                ItemHPBar->SetFillColorAndOpacity(BarColor);
                ItemHPBar->SetVisibility(ESlateVisibility::Visible);
            }
            else
            {
                // No HP tracking for this equipment
                ItemHPBar->SetVisibility(ESlateVisibility::Hidden);
            }
        }
        
        // Show ammo text for weapons that use ammo
        if (BottomTextAmmo)
        {
            if (ItemAssetInfo->bUseAmmo && StoredItemInfo.MaxAmmo > 0)
            {
                FText AmmoText = FText::Format(FText::FromString(TEXT("{0}/{1}")), 
                    FText::AsNumber(StoredItemInfo.CurrentAmmo), 
                    FText::AsNumber(StoredItemInfo.MaxAmmo));
                BottomTextAmmo->SetText(AmmoText);
                BottomTextAmmo->SetVisibility(ESlateVisibility::Visible);
            }
            else
            {
                BottomTextAmmo->SetVisibility(ESlateVisibility::Hidden);
            }
        }
    }
    
    // ================================= Consumable =================================
    else if (ItemAssetInfo->ItemCategory == E_ItemCategory::Consumable)
    {
        // For Consumable items, show quantity like resources
        if (ItemQuantity)
        {
            int32 Quantity = StoredItemInfo.ItemQuantity;
            FText QuantityTextValue = FText::Format(FText::FromString(TEXT("x{0}")),
            FText::AsNumber(Quantity));
            ItemQuantity->SetText(QuantityTextValue);
            ItemQuantity->SetVisibility(ESlateVisibility::Visible);
        }
        
        // Hide equipment-specific elements
        if (BottomTextAmmo)
        {
            BottomTextAmmo->SetVisibility(ESlateVisibility::Hidden);
        }
        if (ItemHPBar)
        {
            ItemHPBar->SetVisibility(ESlateVisibility::Hidden);
        }
    }
    else
    {
        // For all other item types (Quest, Miscellaneous, etc.)
        
        // Show quantity if stackable
        if (ItemQuantity)
        {
            if (ItemAssetInfo->bStackable && StoredItemInfo.ItemQuantity > 1)
            {
                int32 Quantity = StoredItemInfo.ItemQuantity;
                FText QuantityTextValue = FText::Format(FText::FromString(TEXT("x{0}")), FText::AsNumber(Quantity));
                ItemQuantity->SetText(QuantityTextValue);
                ItemQuantity->SetVisibility(ESlateVisibility::Visible);
            }
            else
            {
                ItemQuantity->SetVisibility(ESlateVisibility::Hidden);
            }
        }
        
        // Hide equipment-specific elements
        if (BottomTextAmmo)
        {
            BottomTextAmmo->SetVisibility(ESlateVisibility::Hidden);
        }
        if (ItemHPBar)
        {
            ItemHPBar->SetVisibility(ESlateVisibility::Hidden);
        }
    }
    
    // --- Update TopText ---
    // For all items, hide the top text when an item is present
    if (TopText)
    {
        TopText->SetVisibility(ESlateVisibility::Hidden);
    }

    // ================================= Wearable =================================
    else if (ItemAssetInfo->ItemCategory == E_ItemCategory::Wearable)
    {
        if (ItemQuantity)
        {
            ItemQuantity->SetVisibility(ESlateVisibility::Hidden);
        }


        if (ItemHPBar)
        {
            if (StoredItemInfo.MaxHP > 0.0f)
            {
                float HPPercent = FMath::Clamp(StoredItemInfo.CurrentHP / StoredItemInfo.MaxHP, 0.0f, 1.0f);
                ItemHPBar->SetPercent(HPPercent);
            
                // Color coding based on armor condition
                FLinearColor BarColor;
                if (HPPercent > 0.66f)
                {
                    // Green for good condition (>66%)
                    BarColor = FLinearColor(0.0f, 0.75f, 0.0f);
                }
                else if (HPPercent > 0.33f)
                {
                    // Yellow for moderate condition (33-66%)
                    BarColor = FLinearColor(0.75f, 0.75f, 0.0f);
                }
                else
                {
                    // Red for poor condition (<33%)
                    BarColor = FLinearColor(0.75f, 0.0f, 0.0f);
                }
            
                ItemHPBar->SetFillColorAndOpacity(BarColor);
                ItemHPBar->SetVisibility(ESlateVisibility::Visible);
            }
            else
            {
                // No HP tracking for this wearable
                ItemHPBar->SetVisibility(ESlateVisibility::Hidden);
            }
        }
    
        // Wearable items typically don't have ammo
        if (BottomTextAmmo)
        {
            BottomTextAmmo->SetVisibility(ESlateVisibility::Hidden);
        }

    }

    // ================================= Buildable =================================
    else if (ItemAssetInfo->ItemCategory == E_ItemCategory::Buildable)
    {
        // For Buildable items, show quantity just like Resources
        if (ItemQuantity)
        {
            int32 Quantity = StoredItemInfo.ItemQuantity;
            FText QuantityTextValue = FText::Format(FText::FromString(TEXT("x{0}")), FText::AsNumber(Quantity));
            ItemQuantity->SetText(QuantityTextValue);
            ItemQuantity->SetVisibility(ESlateVisibility::Visible);
        }
    
        // Hide equipment-specific elements
        if (BottomTextAmmo)
        {
            BottomTextAmmo->SetVisibility(ESlateVisibility::Hidden);
        }
        if (ItemHPBar)
        {
            ItemHPBar->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}
