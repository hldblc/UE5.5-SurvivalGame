#include "UI/Widgets/Inventory/DraggedItem.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

UDraggedItem::UDraggedItem()
{
    // Set default values
    ImageIcon = nullptr;
    TextTop = FText::GetEmpty();
    ItemCategory = E_ItemCategory::None;
    Quantity = FText::GetEmpty();
    bUseAmmo = false;
    CurrentAmmo = 0;
    MaxAmmo = 0;
    CurrentHP = 0;
    MaxHP = 0;
    Weight = FText::GetEmpty();
}

void UDraggedItem::NativePreConstruct()
{
    Super::NativePreConstruct();
    
    // Validate widget bindings with specific errors
    if (!ItemWeight)
    {
        UE_LOG(LogTemp, Error, TEXT("DraggedItem: ItemWeight binding is missing!"));
    }
    if (!ItemIcon)
    {
        UE_LOG(LogTemp, Error, TEXT("DraggedItem: ItemIcon binding is missing!"));
    }
    if (!ItemQuantity)
    {
        UE_LOG(LogTemp, Error, TEXT("DraggedItem: ItemQuantity binding is missing!"));
    }
    if (!TopText)
    {
        UE_LOG(LogTemp, Error, TEXT("DraggedItem: TopText binding is missing!"));
    }
    if (!BottomTextAmmo)
    {
        UE_LOG(LogTemp, Error, TEXT("DraggedItem: BottomTextAmmo binding is missing!"));
    }
    if (!ItemHPBar)
    {
        UE_LOG(LogTemp, Error, TEXT("DraggedItem: ItemHPBar binding is missing!"));
    }
    
    // Apply the settings from the exposed properties
    UpdateVisuals();
}

void UDraggedItem::NativeConstruct()
{
    Super::NativeConstruct();
    
    // Apply visual settings when the widget is constructed
    UpdateVisuals();
}

void UDraggedItem::UpdateVisuals()
{
    // Update the item icon
    if (ItemIcon && ImageIcon)
    {
        FSlateBrush Brush;
        Brush.SetResourceObject(ImageIcon);
        Brush.DrawAs = ESlateBrushDrawType::Image;
        Brush.Tiling = ESlateBrushTileType::NoTile;
        
        ItemIcon->SetBrush(Brush);
        ItemIcon->SetVisibility(ESlateVisibility::Visible);
    }
    else if (ItemIcon)
    {
        ItemIcon->SetVisibility(ESlateVisibility::Hidden);
    }
    
    // Update top text
    if (TopText)
    {
        if (!TextTop.IsEmpty())
        {
            TopText->SetText(TextTop);
            TopText->SetVisibility(ESlateVisibility::Hidden);
        }
        else
        {
            TopText->SetVisibility(ESlateVisibility::Hidden);
        }
    }
    
    // Update weight text
    if (ItemWeight)
    {
        if (!Weight.IsEmpty())
        {
            ItemWeight->SetText(Weight);
            ItemWeight->SetVisibility(ESlateVisibility::Visible);
            
        }
        else
        {
            ItemWeight->SetVisibility(ESlateVisibility::Hidden);
        }
    }
    
    // Item category-specific visualizations
    if (ItemCategory == E_ItemCategory::Resource || 
        ItemCategory == E_ItemCategory::Consumable || 
        ItemCategory == E_ItemCategory::Miscellaneous)
    {
        // For resources, consumables, and miscellaneous items:
        // 1. Show quantity
        if (ItemQuantity)
        {
            if (!Quantity.IsEmpty())
            {
                ItemQuantity->SetText(Quantity);
                ItemQuantity->SetVisibility(ESlateVisibility::Visible);
            }
            else
            {
                ItemQuantity->SetVisibility(ESlateVisibility::Hidden);
            }
        }
        
        // 2. Hide HP bar and ammo for these item types
        if (ItemHPBar)
        {
            ItemHPBar->SetVisibility(ESlateVisibility::Hidden);
        }
        
        if (BottomTextAmmo)
        {
            BottomTextAmmo->SetVisibility(ESlateVisibility::Hidden);
        }
    }
    else if (ItemCategory == E_ItemCategory::Equipment)
    {
        // For equipment items:
        // 1. Hide quantity text
        if (ItemQuantity)
        {
            ItemQuantity->SetVisibility(ESlateVisibility::Hidden);
        }
        
        // 2. Show HP bar for equipment
        if (ItemHPBar)
        {
            if (MaxHP > 0)
            {
                float HPPercent = FMath::Clamp(static_cast<float>(CurrentHP) / static_cast<float>(MaxHP), 0.0f, 1.0f);
                ItemHPBar->SetPercent(HPPercent);
                
                // Set color based on HP percentage
                FLinearColor BarColor;
                if (HPPercent > 0.66f)
                {
                    // Green for good condition
                    BarColor = FLinearColor(0.0f, 0.75f, 0.0f);
                }
                else if (HPPercent > 0.33f)
                {
                    // Yellow for moderate condition
                    BarColor = FLinearColor(0.75f, 0.75f, 0.0f);
                }
                else
                {
                    // Red for poor condition
                    BarColor = FLinearColor(0.75f, 0.0f, 0.0f);
                }
                
                ItemHPBar->SetFillColorAndOpacity(BarColor);
                ItemHPBar->SetVisibility(ESlateVisibility::Visible);
            }
            else
            {
                ItemHPBar->SetVisibility(ESlateVisibility::Hidden);
            }
        }
        
        // 3. Show ammo for weapons if applicable
        if (BottomTextAmmo)
        {
            if (bUseAmmo && MaxAmmo > 0)
            {
                FText AmmoText = FText::Format(FText::FromString(TEXT("{0}/{1}")), 
                    FText::AsNumber(CurrentAmmo), 
                    FText::AsNumber(MaxAmmo));
                BottomTextAmmo->SetText(AmmoText);
                BottomTextAmmo->SetVisibility(ESlateVisibility::Visible);
            }
            else
            {
                BottomTextAmmo->SetVisibility(ESlateVisibility::Hidden);
            }
        }
    }
    else
    {
        // For all other item types, set sensible defaults
        if (ItemQuantity)
        {
            ItemQuantity->SetVisibility(ESlateVisibility::Hidden);
        }
        
        if (ItemHPBar)
        {
            ItemHPBar->SetVisibility(ESlateVisibility::Hidden);
        }
        
        if (BottomTextAmmo)
        {
            BottomTextAmmo->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}