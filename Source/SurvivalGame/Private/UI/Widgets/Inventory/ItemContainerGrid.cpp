#include "UI/Widgets/Inventory/ItemContainerGrid.h"
#include "UI/Widgets/Inventory/InventorySlot.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"

UItemContainerGrid::UItemContainerGrid(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , SlotsPerRow(6)
    , TotalSlots(60)
    , ContainerType(E_ContainerType::Inventory)
    , Grid(nullptr) 
{
}

void UItemContainerGrid::NativeConstruct()
{
    Super::NativeConstruct();

    if (SlotsPerRow <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("SlotsPerRow is zero or negative, using default value of 6"));
        SlotsPerRow = 6;
    }
    
    if (TotalSlots <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("TotalSlots is zero or negative, using default value of 60"));
        TotalSlots = 60;
    }
    
    // Rest of your existing code
    UE_LOG(LogTemp, Log, TEXT("ItemContainerGrid::NativeConstruct - Grid: %s, SlotClass: %s"), 
        Grid ? TEXT("Valid") : TEXT("Invalid"),
        InventorySlotClass ? TEXT("Valid") : TEXT("Invalid"));

    // Add debug logging
    UE_LOG(LogTemp, Log, TEXT("ItemContainerGrid::NativeConstruct - Grid: %s, SlotClass: %s"), 
        Grid ? TEXT("Valid") : TEXT("Invalid"),
        InventorySlotClass ? TEXT("Valid") : TEXT("Invalid"));

    if (!Grid)
    {
        UE_LOG(LogTemp, Error, TEXT("Grid panel is not properly bound in UMG!"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("Creating %d inventory slots"), TotalSlots);
    AddSlots(TotalSlots);
}

void UItemContainerGrid::AddSlotToGrid(int32 Index, UInventorySlot* NewSlot)
{
    if (SlotsPerRow <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("AddSlotToGrid: SlotsPerRow is zero or negative, using default value of 6"));
        SlotsPerRow = 6;
    }
    
    if (!Grid)
    {
        UE_LOG(LogTemp, Warning, TEXT("AddSlotToGrid: Grid panel is not bound!"));
        return;
    }
    
    if (!NewSlot)
    {
        UE_LOG(LogTemp, Warning, TEXT("AddSlotToGrid: NewSlot is null!"));
        return;
    }
    
    // Since the blueprint uses a 1-based index, we convert it to 0-based for calculations.
    int32 LocalSlotIndex = Index - 1;
    
    // Add the inventory slot widget to the UniformGridPanel.
    UUniformGridSlot* GridSlot = Cast<UUniformGridSlot>(Grid->AddChildToUniformGrid(NewSlot));
    if (!GridSlot)
    {
        UE_LOG(LogTemp, Warning, TEXT("AddSlotToGrid: Failed to add slot to grid."));
        return;
    }
    
    // Calculate row and column.
    int32 Row = LocalSlotIndex / SlotsPerRow;
    int32 Column = LocalSlotIndex % SlotsPerRow;
    
    // Set the grid slot's row and column.
    GridSlot->SetRow(Row);
    GridSlot->SetColumn(Column);
    
    // Optionally, add the new slot to our internal array.
    Slots.Add(NewSlot);
    
    
}

void UItemContainerGrid::AddSlots(int32 Amount)
{
    if (!InventorySlotClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("AddSlots: InventorySlotClass is not set!"));
        return;
    }
    
    APlayerController* PC = GetOwningPlayer();
    if (!PC)
    {
        UE_LOG(LogTemp, Warning, TEXT("AddSlots: No owning player found!"));
        return;
    }
    
    // IMPORTANT CHANGE: Start from 0 to match data array indices
    for (int32 i = 0; i < Amount; i++)
    {
        // Create the inventory slot widget.
        UInventorySlot* NewSlot = CreateWidget<UInventorySlot>(PC, InventorySlotClass);
        if (NewSlot)
        {
            // Set the slot's container type.
            NewSlot->ContainerType = ContainerType;
            // Set the slot's item index to match the 0-based data array
            NewSlot->ItemIndex = i;
            
            // Call AddSlotToGrid with adjusted index for visual layout
            AddSlotToGrid(i + 1, NewSlot);
            
            UE_LOG(LogTemp, Log, TEXT("AddSlots: Created slot with index %d"), i);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("AddSlots: Failed to create an inventory slot widget."));
        }
    }
}