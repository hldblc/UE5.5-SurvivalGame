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
#include "Interfaces/PlayerInterface.h"
#include "Internationalization/Text.h"
#include "Library/ItemAssetCache.h"
#include "UI/Widgets/Inventory/DraggedItem.h"
#include "UI/Widgets/Inventory/Operations/ItemDrag.h"

static TMap<int32, FString> LastUpdateKeys;


// ===============================================================================================
// Constructor
// ===============================================================================================
UInventorySlot::UInventorySlot()
: ItemIndex(0)
, ContainerType(E_ContainerType::None)
, bHasItemInSlot(false)
, ItemAssetInfo(nullptr)
, ItemWeight(nullptr)
, ItemIcon(nullptr)
, ItemQuantity(nullptr)
, TopText(nullptr)
, BottomTextAmmo(nullptr)
, ItemHPBar(nullptr)
{
    
}


// ===============================================================================================
// NativePreConstruct
// ===============================================================================================
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




// ===============================================================================================
// NativeConstruct
// ===============================================================================================
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



// ===============================================================================================
// NativeOnMouseButtonDown
// ===============================================================================================
FReply UInventorySlot::NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        // Replicate the Blueprint’s "SetDragInput Pressed" path by returning Handled.
        return FReply::Handled();
    }
    return Super::NativeOnMouseButtonDown(MyGeometry, MouseEvent);
}




// ===============================================================================================
// NativeOnPreviewMouseButtonDown
// ===============================================================================================
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




// ===============================================================================================
// NativeOnDragDetected
// ===============================================================================================
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
    DragVisual->TextTop       = FText::AsNumber(ItemAssetInfo->ItemDamage);
    DragVisual->ItemCategory  = ItemAssetInfo->ItemCategory;
    DragVisual->Quantity      = FText::AsNumber(StoredItemInfo.ItemQuantity);
    DragVisual->bUseAmmo      = ItemAssetInfo->bUseAmmo;
    DragVisual->CurrentAmmo   = StoredItemInfo.CurrentAmmo;
    DragVisual->MaxAmmo       = StoredItemInfo.MaxAmmo;
    DragVisual->CurrentHP     = StoredItemInfo.CurrentHP;
    DragVisual->MaxHP         = StoredItemInfo.MaxHP;
    
    // Calculate and format total weight with specific formatting options
    float TotalWeight = StoredItemInfo.ItemQuantity * ItemAssetInfo->ItemWeight;
    FNumberFormattingOptions FormatOptions;
    FormatOptions.MinimumFractionalDigits = 1;
    FormatOptions.MaximumFractionalDigits = 3;
    FormatOptions.UseGrouping = true;
    FormatOptions.AlwaysSign = false;
    FormatOptions.MinimumIntegralDigits = 1;
    FormatOptions.MaximumIntegralDigits = 324;
    DragVisual->Weight = FText::AsNumber(TotalWeight, &FormatOptions);
    
    // Create the drag operation and assign the drag visual
    UItemDrag* DragOperation = NewObject<UItemDrag>();
    if (!DragOperation)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create ItemDrag operation"));
        return;
    }
    
    DragOperation->DefaultDragVisual = DragVisual;
    DragOperation->SlotIndex         = ItemIndex;
    DragOperation->FromContainer     = ContainerType;
    DragOperation->ItemCategory      = ItemAssetInfo->ItemCategory;
    DragOperation->ArmorType         = ItemAssetInfo->ArmorType;
    DragOperation->Pivot             = EDragPivot::MouseDown;
    
    OutOperation = DragOperation;
}



// ===============================================================================================
// NativeOnDrop
// ===============================================================================================
bool UInventorySlot::NativeOnDrop(const FGeometry& MyGeometry, const FDragDropEvent& PointerEvent, UDragDropOperation* Operation)
{
    // Cast the operation to our custom ItemDrag class
    UItemDrag* ItemDragOperation = Cast<UItemDrag>(Operation);

        if (!ItemDragOperation)
        {
            return false;
        }

    // Create local variables from the drag operation
    int32 LocalSlotIndex = ItemDragOperation->SlotIndex;
    E_ContainerType LocalFromContainer = ItemDragOperation->FromContainer;
    E_ItemCategory LocalItemCategory = ItemDragOperation->ItemCategory;
    E_ItemType LocalItemType = ItemDragOperation->ItemType;
    E_ArmorType LocalArmorType = ItemDragOperation->ArmorType;

    APawn* PlayerCharacter = GetOwningPlayerPawn();
    if (!IsValid(PlayerCharacter))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to get valid Player Character in InventorySlot::NativeOnDrop"));
        return false;
    }

    if (PlayerCharacter->Implements<UPlayerInterface>())
    {
        IPlayerInterface::Execute_OnSlotDrop(
            PlayerCharacter,
            ItemIndex,
            LocalSlotIndex,
            ContainerType,
            LocalFromContainer,
            LocalArmorType
            );
        return true;
    }
            
    return false;
}






// ===============================================================================================
// OnItemAssetLoaded
// ===============================================================================================
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




// ===============================================================================================
// UpdateUIElements
// ===============================================================================================
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
        FormatOptions.MaximumFractionalDigits = 3;
        FormatOptions.UseGrouping = true;
        FormatOptions.AlwaysSign = false;
        FormatOptions.MinimumIntegralDigits = 1;
        FormatOptions.MaximumIntegralDigits = 324;
        FText WeightTextValue = FText::AsNumber(TotalWeight, &FormatOptions);
        ItemWeight->SetText(WeightTextValue);
        ItemWeight->SetVisibility(ESlateVisibility::Visible);
    }

    // --- Update ItemIcon using cached textures ---
    if (ItemIcon)
    {
        FSoftObjectPath IconPath = ItemAssetInfo->ItemIcon.ToSoftObjectPath();
        
        // Try to get from cache first
        UTexture2D* IconTexture = UItemAssetCache::GetCachedItemIcon(IconPath);
        if (IconTexture)
        {
            // Use cached texture
            FSlateBrush Brush;
            Brush.SetResourceObject(IconTexture);
            Brush.DrawAs = ESlateBrushDrawType::Image;
            Brush.Tiling = ESlateBrushTileType::NoTile;
            
            ItemIcon->SetBrush(Brush);
            ItemIcon->SetVisibility(ESlateVisibility::Visible);
            
            UE_LOG(LogTemp, Verbose, TEXT("UpdateUIElements: Using cached icon texture"));
        }
        else
        {
            // Request async load with caching
            UItemAssetCache::RequestItemIconAsync(IconPath, 
                FOnTextureLoaded::CreateLambda([this](UTexture2D* LoadedTexture) {
                    if (LoadedTexture && ItemIcon)
                    {
                        FSlateBrush Brush;
                        Brush.SetResourceObject(LoadedTexture);
                        Brush.DrawAs = ESlateBrushDrawType::Image;
                        Brush.Tiling = ESlateBrushTileType::NoTile;
                        
                        ItemIcon->SetBrush(Brush);
                        ItemIcon->SetVisibility(ESlateVisibility::Visible);
                    }
                })
            );
        }
    }
    
    // --- Update TopText ---
    if (TopText)
    {
        TopText->SetVisibility(ESlateVisibility::Hidden);
    }
    
    // --- Reset visibility of components we'll conditionally show ---
    if (ItemQuantity)
    {
        ItemQuantity->SetVisibility(ESlateVisibility::Hidden);
    }
    
    if (BottomTextAmmo)
    {
        BottomTextAmmo->SetVisibility(ESlateVisibility::Hidden);
    }
    
    if (ItemHPBar)
    {
        ItemHPBar->SetVisibility(ESlateVisibility::Hidden);
    }

    // Update category-specific UI elements
    switch (ItemAssetInfo->ItemCategory)
    {
        case E_ItemCategory::Resource:
        case E_ItemCategory::Consumable:
        case E_ItemCategory::Buildable:
            // Update quantity text for items that show quantity
            if (ItemQuantity)
            {
                int32 Quantity = StoredItemInfo.ItemQuantity;
                FText QuantityTextValue = FText::Format(FText::FromString(TEXT("x{0}")), FText::AsNumber(Quantity));
                ItemQuantity->SetText(QuantityTextValue);
                ItemQuantity->SetVisibility(ESlateVisibility::Visible);
            }
            break;
            
        case E_ItemCategory::Equipment:
            // Show durability bar if HP values are valid
            if (ItemHPBar && StoredItemInfo.MaxHP > 0.0f)
            {
                float HPPercent = FMath::Clamp(StoredItemInfo.CurrentHP / StoredItemInfo.MaxHP, 0.0f, 1.0f);
                ItemHPBar->SetPercent(HPPercent);
                
                // Set color based on durability percentage
                FLinearColor BarColor;
                if (HPPercent > 0.66f)
                {
                    BarColor = FLinearColor(0.0f, 0.75f, 0.0f);  // Green
                }
                else if (HPPercent > 0.33f)
                {
                    BarColor = FLinearColor(0.75f, 0.75f, 0.0f); // Yellow
                }
                else
                {
                    BarColor = FLinearColor(0.75f, 0.0f, 0.0f);  // Red
                }
                
                ItemHPBar->SetFillColorAndOpacity(BarColor);
                ItemHPBar->SetVisibility(ESlateVisibility::Visible);
            }
            
            // Show ammo text for weapons that use ammo
            if (BottomTextAmmo && ItemAssetInfo->bUseAmmo && StoredItemInfo.MaxAmmo > 0)
            {
                FText AmmoText = FText::Format(FText::FromString(TEXT("{0}/{1}")), 
                    FText::AsNumber(StoredItemInfo.CurrentAmmo), 
                    FText::AsNumber(StoredItemInfo.MaxAmmo));
                BottomTextAmmo->SetText(AmmoText);
                BottomTextAmmo->SetVisibility(ESlateVisibility::Visible);
            }
            break;
            
        case E_ItemCategory::Wearable:
            // Show durability bar if HP values are valid
            if (ItemHPBar && StoredItemInfo.MaxHP > 0.0f)
            {
                float HPPercent = FMath::Clamp(StoredItemInfo.CurrentHP / StoredItemInfo.MaxHP, 0.0f, 1.0f);
                ItemHPBar->SetPercent(HPPercent);
                
                // Color coding based on armor condition
                FLinearColor BarColor;
                if (HPPercent > 0.66f)
                {
                    BarColor = FLinearColor(0.0f, 0.75f, 0.0f);  // Green
                }
                else if (HPPercent > 0.33f)
                {
                    BarColor = FLinearColor(0.75f, 0.75f, 0.0f); // Yellow
                }
                else
                {
                    BarColor = FLinearColor(0.75f, 0.0f, 0.0f);  // Red
                }
                
                ItemHPBar->SetFillColorAndOpacity(BarColor);
                ItemHPBar->SetVisibility(ESlateVisibility::Visible);
            }
            break;
            
        default:
            // For all other item types (Quest, Miscellaneous, etc.)
            if (ItemQuantity && ItemAssetInfo->bStackable && StoredItemInfo.ItemQuantity > 1)
            {
                int32 Quantity = StoredItemInfo.ItemQuantity;
                FText QuantityTextValue = FText::Format(FText::FromString(TEXT("x{0}")), FText::AsNumber(Quantity));
                ItemQuantity->SetText(QuantityTextValue);
                ItemQuantity->SetVisibility(ESlateVisibility::Visible);
            }
            break;
    }
}

// ===============================================================================================
// UpdateSlot
// ===============================================================================================
void UInventorySlot::UpdateSlot(FItemStructure ItemInfo)
{
    // Generate a unique identifier for this update
    FString UpdateKey = FString::Printf(TEXT("%s_%d"), *ItemInfo.RegistryKey.ToString(), ItemInfo.ItemQuantity);
    
    // Check for duplicate updates to the same slot
    FString* ExistingKey = LastUpdateKeys.Find(ItemIndex);
    if (ExistingKey && *ExistingKey == UpdateKey && bHasItemInSlot)
    {
        UE_LOG(LogTemp, Verbose, TEXT("UpdateSlot: Skipping duplicate update for slot %d"), ItemIndex);
        return;
    }
    
    // Record this update
    LastUpdateKeys.FindOrAdd(ItemIndex) = UpdateKey;
    
    UE_LOG(LogTemp, Log, TEXT("UpdateSlot: Starting for slot %d"), ItemIndex);
    
    // Step 1: Mark this slot as occupied
    bHasItemInSlot = true;
    
    // Step 2: Save the incoming item info for later UI updates
    StoredItemInfo = ItemInfo;
    
    // Skip if registry key is invalid
    if (ItemInfo.RegistryKey.IsNone())
    {
        UE_LOG(LogTemp, Verbose, TEXT("UpdateSlot: Empty item received for slot %d, clearing"), ItemIndex);
        ClearSlot();
        return;
    }
    
    // Get the asset path
    FSoftObjectPath AssetPath = ItemInfo.ItemAsset.ToSoftObjectPath();
    
    if (AssetPath.IsNull())
    {
        // Try to resolve via asset manager if path is missing
        UAssetManager* AssetManager = UAssetManager::GetIfInitialized();
        if (AssetManager)
        {
            FPrimaryAssetId ItemId = FPrimaryAssetId(FPrimaryAssetType("Item"), ItemInfo.RegistryKey);
            FSoftObjectPath ItemPath = AssetManager->GetPrimaryAssetPath(ItemId);
            
            if (!ItemPath.IsNull())
            {
                AssetPath = ItemPath;
                StoredItemInfo.ItemAsset = TSoftObjectPtr<UItemInfo>(ItemPath);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("UpdateSlot: Failed to resolve asset path for registry key: %s"), 
                       *ItemInfo.RegistryKey.ToString());
                ClearSlot();
                return;
            }
        }
    }
    
    UE_LOG(LogTemp, Log, TEXT("UpdateSlot: Attempting to load asset at path: %s"), *AssetPath.ToString());
    
    // Check if we already have this asset loaded via cache
    UItemInfo* CachedInfo = Cast<UItemInfo>(AssetPath.ResolveObject());
    if (CachedInfo)
    {
        UE_LOG(LogTemp, Log, TEXT("UpdateSlot: Using already loaded asset: %s"), *CachedInfo->GetName());
        ItemAssetInfo = CachedInfo;
        UpdateUIElements();
        return;
    }
    
    // Try to get from global cache if available
    UItemInfo* GlobalCachedInfo = UItemAssetCache::GetCachedItemInfo(AssetPath);
    if (GlobalCachedInfo)
    {
        UE_LOG(LogTemp, Log, TEXT("UpdateSlot: Using cached item info from global cache: %s"), *GlobalCachedInfo->GetName());
        ItemAssetInfo = GlobalCachedInfo;
        UpdateUIElements();
        return;
    }
    
    // Asset isn't loaded or cached - request async load via asset cache
    UItemAssetCache::RequestItemInfoAsync(AssetPath, 
        FOnItemInfoLoaded::CreateLambda([this](UItemInfo* LoadedItemInfo) {
            if (LoadedItemInfo)
            {
                ItemAssetInfo = LoadedItemInfo;
                UpdateUIElements();
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("UpdateSlot: Failed to load item info for slot %d"), ItemIndex);
            }
        })
    );
}


// ===============================================================================================
// ClearSlot
// ===============================================================================================
void UInventorySlot::ClearSlot()
{
    // Skip processing if slot is already empty
    if (!bHasItemInSlot && !ItemAssetInfo)
    {
        return;
    }
    
    // Reset slot data (batch these operations)
    bHasItemInSlot = false;
    ItemAssetInfo = nullptr;
    StoredItemInfo = FItemStructure();
    
    // Use a single visibility update pass to reduce property changed notifications
    const ESlateVisibility HiddenVisibility = ESlateVisibility::Hidden;
    
    // Only create the empty brush once if needed
    static FSlateBrush EmptyBrush;
    static bool bEmptyBrushInitialized = false;
    if (!bEmptyBrushInitialized)
    {
        EmptyBrush.DrawAs = ESlateBrushDrawType::NoDrawType;
        bEmptyBrushInitialized = true;
    }
    
    if (ItemIcon)
    {
        ItemIcon->SetBrush(EmptyBrush);
        ItemIcon->SetVisibility(HiddenVisibility);
    }
    
    // Use a cached empty text to avoid creating multiple instances
    static const FText EmptyText = FText::GetEmpty();
    
    // Batch UI updates - set content first, then visibility in one pass
    if (ItemQuantity)
    {
        ItemQuantity->SetText(EmptyText);
        ItemQuantity->SetVisibility(HiddenVisibility);
    }
    
    if (ItemWeight)
    {
        ItemWeight->SetText(EmptyText);
        ItemWeight->SetVisibility(HiddenVisibility);
    }
    
    if (BottomTextAmmo)
    {
        BottomTextAmmo->SetText(EmptyText);
        BottomTextAmmo->SetVisibility(HiddenVisibility);
    }
    
    if (ItemHPBar)
    {
        ItemHPBar->SetPercent(0.0f);
        ItemHPBar->SetVisibility(HiddenVisibility);
    }
    
    // Only element we show
    if (TopText)
    {
        TopText->SetText(EmptyText);
        TopText->SetVisibility(ESlateVisibility::Visible);
    }
    
    // Disable detailed logging in shipping builds
#if !UE_BUILD_SHIPPING
    UE_LOG(LogTemp, Verbose, TEXT("ClearSlot: Cleared slot %d"), ItemIndex);
#endif
}




