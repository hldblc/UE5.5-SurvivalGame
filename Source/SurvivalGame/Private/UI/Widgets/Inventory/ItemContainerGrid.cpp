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
    
    UE_LOG(LogTemp, Log, TEXT("AddSlotToGrid: Added slot at 0-based index %d (Row: %d, Column: %d)"), LocalSlotIndex, Row, Column);
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
    
    // Loop from 1 to Amount (1-based index as per your Blueprint instructions).
    for (int32 i = 1; i <= Amount; i++)
    {
        // Create the inventory slot widget.
        UInventorySlot* NewSlot = CreateWidget<UInventorySlot>(PC, InventorySlotClass);
        if (NewSlot)
        {
            // Set the slot's container type.
            NewSlot->ContainerType = ContainerType;
            // Set the slot's item index.
            NewSlot->ItemIndex = i;
            
            // Call AddSlotToGrid with the 1-based index and the new slot.
            AddSlotToGrid(i, NewSlot);
            
            // Optionally, also add it to our Slots array (if not already added in AddSlotToGrid).
            // (Our AddSlotToGrid already adds it, so this is optional.)
            // Slots.Add(NewSlot);
            
            UE_LOG(LogTemp, Log, TEXT("AddSlots: Created and added slot with 1-based index %d"), i);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("AddSlots: Failed to create an inventory slot widget."));
        }
    }
}
