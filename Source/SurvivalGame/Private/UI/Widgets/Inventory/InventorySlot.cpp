#include "UI/Widgets/Inventory/InventorySlot.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Data/PrimaryData/ItemInfo.h"  
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Internationalization/Text.h"



void UInventorySlot::NativePreConstruct()
{
    Super::NativePreConstruct();
    
    // Validate widget bindings
    if (!ItemWeight || !ItemIcon || !ItemQuantity || !TopText || !BottomTextAmmo || !ItemHPBar)
    {
        UE_LOG(LogTemp, Error, TEXT("InventorySlot: Required widget bindings are missing!"));
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


void UInventorySlot::UpdateSlot(FItemStructure ItemInfo)
{
    // Step 1: Mark this slot as occupied—empty slots are so last season.
    bHasItemInSlot = true;
    // Save the incoming item info for later UI updates.
    StoredItemInfo = ItemInfo;
    
    // Step 2: Build a soft pointer from the item's soft asset reference using the new API.
    TSoftObjectPtr<UPrimaryDataAsset> SoftItemAsset(ItemInfo.ItemAsset.ToSoftObjectPath());
    
    // Step 3: If the asset is already loaded, grab it right away.
    if (SoftItemAsset.IsValid())
    {
        // Cast to our desired type (UItemInfo) if possible.
        ItemAssetInfo = Cast<UItemInfo>(SoftItemAsset.Get());
        UE_LOG(LogTemp, Log, TEXT("UpdateSlot: Asset already loaded. ItemAssetInfo set on the fly."));
        // Update the UI elements with the new item details.
        UpdateUIElements();
    }
    else
    {
        // Step 4: Asset isn't loaded? Time to request an async load.
        FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
        Streamable.RequestAsyncLoad(
            SoftItemAsset.ToSoftObjectPath(),
            FStreamableDelegate::CreateUObject(this, &UInventorySlot::OnItemAssetLoaded)
        );
        UE_LOG(LogTemp, Log, TEXT("UpdateSlot: Asset not loaded. Initiating async load..."));
    }
}

void UInventorySlot::OnItemAssetLoaded()
{
    // Rebuild the soft pointer from our stored info (after async load).
    TSoftObjectPtr<UPrimaryDataAsset> SoftItemAsset(StoredItemInfo.ItemAsset.ToSoftObjectPath());
    if (SoftItemAsset.IsValid())
    {
        // Cast to UItemInfo.
        ItemAssetInfo = Cast<UItemInfo>(SoftItemAsset.Get());
        UE_LOG(LogTemp, Log, TEXT("OnItemAssetLoaded: Async load complete. ItemAssetInfo is now live."));
        // Now update the UI elements.
        UpdateUIElements();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("OnItemAssetLoaded: Uh oh—asset load failed. Check your settings, champ."));
    }
}

void UInventorySlot::UpdateUIElements()
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
    if (ItemIcon && ItemAssetInfo->ItemIcon.IsValid())
    {
        FSlateBrush Brush;
        Brush.SetResourceObject(ItemAssetInfo->ItemIcon.Get());
        ItemIcon->SetBrush(Brush);
        ItemIcon->SetVisibility(ESlateVisibility::Visible);
    }
    
    // --- Update ItemQuantity Based on Item Category (Resource Only) ---
    // We are currently only implementing the Resource branch.
    // (Assuming that UItemInfo has an "ItemCategory" property and that E_ItemCategory::Resource exists.)
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
}
