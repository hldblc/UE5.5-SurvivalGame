#include "UI/Widgets/Hotbar/PlayerHotbar.h"
#include "Components/Border.h"
#include "UI/Widgets/Inventory/ItemContainerGrid.h"
#include "CommonUI/Public/CommonActionWidget.h"

UPlayerHotbar::UPlayerHotbar(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , ActiveSlotIndex(-1)
    , HotbarSlotCount(8)
{
    // Default initialization
}

void UPlayerHotbar::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    
    // No specific initialization needed in this phase
}

void UPlayerHotbar::NativeConstruct()
{
    Super::NativeConstruct();
    
    // Validate essential widget references are bound correctly
    if (!ValidateWidgetReferences())
    {
        UE_LOG(LogTemp, Error, TEXT("UPlayerHotbar: Failed to validate widget references. Check UMG bindings."));
        return;
    }
    
    // Initialize array of hotbar slot references for easier access
    HotbarSlots.Empty(HotbarSlotCount);
    
    // Add all hotbar slots to our array for easier management
    HotbarSlots.Add(Hotbar01);
    HotbarSlots.Add(Hotbar02);
    HotbarSlots.Add(Hotbar03);
    HotbarSlots.Add(Hotbar04);
    HotbarSlots.Add(Hotbar05);
    HotbarSlots.Add(Hotbar06);
    HotbarSlots.Add(Hotbar07);
    HotbarSlots.Add(Hotbar08);
    
    // Initialize with the first slot as active
    SetActiveHotbarSlot(0);
}

bool UPlayerHotbar::ValidateWidgetReferences() const
{
    // Check essential widget references
    bool bIsValid = true;
    
    if (!ItemContainerGrid)
    {
        UE_LOG(LogTemp, Error, TEXT("%s: ItemContainerGrid not found! Make sure it's bound in the Blueprint."), *GetName());
        bIsValid = false;
    }
    
    // Check that we have all our hotbar slots
    if (!Hotbar01 || !Hotbar02 || !Hotbar03 || !Hotbar04 ||
        !Hotbar05 || !Hotbar06 || !Hotbar07 || !Hotbar08)
    {
        UE_LOG(LogTemp, Error, TEXT("%s: One or more hotbar slot widgets not found! Check UMG bindings."), *GetName());
        bIsValid = false;
    }
    
    return bIsValid;
}

void UPlayerHotbar::SetActiveHotbarSlot(int32 SlotIndex)
{
    // Validate slot index
    if (SlotIndex < 0 || SlotIndex >= HotbarSlotCount)
    {
        UE_LOG(LogTemp, Warning, TEXT("UPlayerHotbar::SetActiveHotbarSlot: Invalid slot index %d"), SlotIndex);
        return;
    }
    
    // Update active slot index
    ActiveSlotIndex = SlotIndex;
    
    // Update visuals for all slots
    UpdateHotbarSlotVisuals();
}

void UPlayerHotbar::UpdateHotbarSlotVisuals()
{
    // Common visual styles for active vs inactive slots
    static const FLinearColor ActiveColor(1.0f, 0.8f, 0.2f, 1.0f);    // Golden highlighted color
    static const FLinearColor InactiveColor(0.5f, 0.5f, 0.5f, 0.7f);  // Gray for inactive slots
    
    // Loop through all slots and update their appearance
    for (int32 SlotIndex = 0; SlotIndex < HotbarSlots.Num(); ++SlotIndex)
    {
        UCommonActionWidget* ActionWidget = HotbarSlots[SlotIndex];
        if (!ActionWidget)
        {
            continue;
        }
        
        const bool bIsActive = (SlotIndex == ActiveSlotIndex);
        
        // Apply visual feedback to indicate the active state
        // We need to use the FSlateColor constructor as CommonActionWidget likely uses this instead of FLinearColor
        FSlateColor SlateColor = bIsActive ? FSlateColor(ActiveColor) : FSlateColor(InactiveColor);
        
        // Set the opacity/visibility of the widget based on active state
        const float ActiveOpacity = 1.0f;
        const float InactiveOpacity = 0.7f;
        ActionWidget->SetRenderOpacity(bIsActive ? ActiveOpacity : InactiveOpacity);
        
        // Instead of changing brush size, we'll use render scale to make active items appear larger
        if (bIsActive)
        {
            ActionWidget->SetRenderScale(FVector2D(1.15f, 1.15f)); // 15% larger when active
        }
        else
        {
            ActionWidget->SetRenderScale(FVector2D(1.0f, 1.0f)); // Normal size when inactive
        }
    }
}