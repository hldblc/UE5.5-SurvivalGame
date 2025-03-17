#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "PlayerHotbar.generated.h"

class USizeBox;
class UItemContainerGrid;
class UCommonActionWidget;

/**
 * @brief Player hotbar widget displaying equipped items.
 *
 * This widget displays the quick-access hotbar with slots for player-equipped items.
 * Players can select and use these items during gameplay.
 */
UCLASS(BlueprintType, Blueprintable, meta = (DisplayName = "Player Hotbar"))
class SURVIVALGAME_API UPlayerHotbar : public UCommonUserWidget
{
    GENERATED_BODY()

public:
    UPlayerHotbar(const FObjectInitializer& ObjectInitializer);

    /** Initialize the widget and bindings */
    virtual void NativeConstruct() override;
    
    /** Called after the underlying slate widget is constructed */
    virtual void NativeOnInitialized() override;
    
    /** Returns the item container grid */
    UFUNCTION(BlueprintCallable, Category = "Hotbar|Items")
    UItemContainerGrid* GetItemContainerGrid() const { return ItemContainerGrid; }

    /** Set active hotbar slot */
    UFUNCTION(BlueprintCallable, Category = "Hotbar|Selection")
    void SetActiveHotbarSlot(int32 SlotIndex);
    
    /** Get active hotbar slot index */
    UFUNCTION(BlueprintPure, Category = "Hotbar|Selection")
    int32 GetActiveHotbarSlot() const { return ActiveSlotIndex; }

protected:
    /** Item container grid for the hotbar */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UItemContainerGrid> ItemContainerGrid;

    /** Hotbar slot 1 */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCommonActionWidget> Hotbar01;

    /** Hotbar slot 2 */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCommonActionWidget> Hotbar02;

    /** Hotbar slot 3 */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCommonActionWidget> Hotbar03;

    /** Hotbar slot 4 */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCommonActionWidget> Hotbar04;

    /** Hotbar slot 5 */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCommonActionWidget> Hotbar05;

    /** Hotbar slot 6 */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCommonActionWidget> Hotbar06;

    /** Hotbar slot 7 */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCommonActionWidget> Hotbar07;

    /** Hotbar slot 8 */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCommonActionWidget> Hotbar08;

    /** Hotbar swap button */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCommonActionWidget> HotbarSwap;

    /** Currently selected hotbar slot index (0-based) */
    UPROPERTY(BlueprintReadOnly, Category = "Hotbar")
    int32 ActiveSlotIndex;

    /** Total number of hotbar slots */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hotbar|Config")
    int32 HotbarSlotCount;

private:
    /** Validates all required widget references */
    bool ValidateWidgetReferences() const;

    /** Updates the visual state of the hotbar slots to reflect the active selection */
    void UpdateHotbarSlotVisuals();
    
    /** Array of all hotbar slot widgets for easier access */
    UPROPERTY(Transient)
    TArray<TObjectPtr<UCommonActionWidget>> HotbarSlots;
};