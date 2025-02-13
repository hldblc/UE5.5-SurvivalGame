#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "InventorySlot.h"
#include "Enums/ContainerType.h"  // Ensure this is the correct path for your enum
#include "ItemContainerGrid.generated.h"

class UInventorySlot;
class UUniformGridPanel;

/**
 * @brief Widget for displaying a grid of inventory slots.
 *
 * Blueprint hierarchy:
 * W_ItemContainerGrid
 * └─ Scroll Box
 *    └─ Grid (UniformGridPanel)
 *
 * Exposed variables:
 * - SlotsPerRow (int32)
 * - TotalSlots (int32)
 * - ContainerType (E_ContainerType)
 */
UCLASS(BlueprintType, Blueprintable)
class SURVIVALGAME_API UItemContainerGrid : public UCommonUserWidget
{
    GENERATED_BODY()

public:
    // Constructor.
    UItemContainerGrid(const FObjectInitializer& ObjectInitializer);

    // Called when the widget is constructed.
    virtual void NativeConstruct() override;

public:
    UFUNCTION(BlueprintCallable, Category = "Debug")
    void DebugSlotConfiguration()
    {
        UE_LOG(LogTemp, Log, TEXT("ItemContainerGrid Debug:"));
        UE_LOG(LogTemp, Log, TEXT("SlotsPerRow: %d"), SlotsPerRow);
        UE_LOG(LogTemp, Log, TEXT("TotalSlots: %d"), TotalSlots);
        UE_LOG(LogTemp, Log, TEXT("Grid Valid: %s"), Grid ? TEXT("True") : TEXT("False"));
        UE_LOG(LogTemp, Log, TEXT("InventorySlotClass Valid: %s"), InventorySlotClass ? TEXT("True") : TEXT("False"));
        UE_LOG(LogTemp, Log, TEXT("Current Slots Count: %d"), Slots.Num());
    }

    /** Number of inventory slots per row. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Container")
    int32 SlotsPerRow;

    /** Total number of inventory slots in the grid. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Container")
    int32 TotalSlots;

    /** The container type (e.g., Inventory, Storage, etc.). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Container")
    E_ContainerType ContainerType;

    /** Blueprint class for the inventory slot widget (W_InventorySlot). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Container")
    TSubclassOf<UInventorySlot> InventorySlotClass;

    /** Array holding references to the inventory slot widgets. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Container")
    TArray<UInventorySlot*> Slots;

    /** The UniformGridPanel that holds the inventory slots.
        Bind this variable in your W_ItemContainerGrid blueprint to the Grid (UniformGridPanel) widget. */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UUniformGridPanel> Grid;

    /**
     * Adds a single inventory slot widget to the grid.
     * @param Index - The 1-based index for the slot.
     * @param NewSlot - The inventory slot widget to add.
     */
    UFUNCTION(BlueprintCallable, Category = "Item Container")
    void AddSlotToGrid(int32 Index, UInventorySlot* NewSlot);

    /**
     * Creates and adds a number of inventory slot widgets to the grid.
     * This function loops from 1 to Amount, creates each widget, sets its properties,
     * and calls AddSlotToGrid.
     * @param Amount - The number of slots to create.
     */
    UFUNCTION(BlueprintCallable, Category = "Item Container")
    void AddSlots(int32 Amount);
};