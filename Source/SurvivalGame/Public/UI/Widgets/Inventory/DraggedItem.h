#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "SurvivalGame/Public/Enums/ItemEnums.h"
#include "Runtime/UMG/Public/Components/TextBlock.h"
#include "CommonUI/Public/CommonBorder.h"
#include "Components/ProgressBar.h"
#include "DraggedItem.generated.h"

/**
 * Widget used for drag-and-drop operations from inventory slots.
 * This is a simplified version of InventorySlot that only displays item information.
 */
UCLASS(Blueprintable, meta = (DisplayName = "Dragged Item"))
class SURVIVALGAME_API UDraggedItem : public UCommonButtonBase
{
    GENERATED_BODY()

public:
    UDraggedItem();

    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;

    // Helper function to update all visuals based on exposed properties
    void UpdateVisuals();

    // --- Exposed properties for drag visual (set when creating the widget) ---
    
    /** The item's icon texture */
    UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
    UTexture2D* ImageIcon;
    
    /** Top text to display (usually item name) */
    UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
    FText TextTop;
    
    /** Item category */
    UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
    E_ItemCategory ItemCategory;
    
    /** Item quantity text (e.g. "x5") */
    UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
    FText Quantity;
    
    /** Whether the item uses ammo */
    UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
    bool bUseAmmo;
    
    /** Current ammo count */
    UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
    int32 CurrentAmmo;
    
    /** Maximum ammo capacity */
    UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
    int32 MaxAmmo;
    
    /** Current item durability/health */
    UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
    int32 CurrentHP;
    
    /** Maximum item durability/health */
    UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
    int32 MaxHP;
    
    /** Item weight text */
    UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
    FText Weight;

    // --- UI Widget Bindings ---

    /** The text block that displays the total weight */
    UPROPERTY(meta = (BindWidget))
    UTextBlock* ItemWeight;

    /** The border that shows the item icon */
    UPROPERTY(meta = (BindWidget))
    UCommonBorder* ItemIcon;

    /** The text block that shows the item quantity (formatted like "x3") */
    UPROPERTY(meta = (BindWidget))
    UTextBlock* ItemQuantity;

    /** A text block that is hidden when an item is present */
    UPROPERTY(meta = (BindWidget))
    UTextBlock* TopText;

    /** The text block that shows the ammo information (e.g., "30/30") */
    UPROPERTY(meta = (BindWidget))
    UTextBlock* BottomTextAmmo;

    /** The progress bar that shows the item's HP */
    UPROPERTY(meta = (BindWidget))
    UProgressBar* ItemHPBar;
};