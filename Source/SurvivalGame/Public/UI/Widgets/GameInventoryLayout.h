#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "UI/Widgets/InventoryWidget.h"
#include "GameInventoryLayout.generated.h"

class UInventoryWidget;
class UCommonButtonBase;
class APlayerController;

/**
 * @brief Layout widget managing the game's inventory interface tabs.
 * 
 * This class is designed for use with CommonUI (UCommonActivatableWidget).
 * The associated Blueprint is typically named WBP_GameInventoryLayout.
 */
UCLASS(BlueprintType, Blueprintable, meta = (DisplayName = "Game Inventory Layout"))
class SURVIVALGAME_API UGameInventoryLayout : public UCommonActivatableWidget
{
    GENERATED_BODY()

public:
    /** Constructor using the object initializer. */
    UGameInventoryLayout(const FObjectInitializer& ObjectInitializer);

    /**
     * @brief Retrieves the main inventory widget.
     * 
     * This is used by the PlayerController or other classes that need direct access
     * to the UInventoryWidget sub-widget for slot retrieval, etc.
     */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    UInventoryWidget* GetInventoryWidget() const { return InventoryWidget; }

protected:
    /**
     * Called when the widget is fully constructed (after the underlying Slate widget is created).
     * We use this to:
     * - Cache the owning PlayerController.
     * - Bind button clicks (e.g., InventoryExitButton).
     */
    virtual void NativeConstruct() override;

    /**
     * Called by CommonUI when this widget is deactivated (e.g., we call DeactivateWidget()).
     * We use this to signal the PlayerController to close/hide the inventory UI properly.
     */
    virtual void NativeOnDeactivated() override;
    
    /** 
     * Reference to the main inventory widget. 
     * Bind this in your WBP_GameInventoryLayout Blueprint (the InventoryWidget).
     */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UInventoryWidget> InventoryWidget;

    /** 
     * Reference to the exit button for the inventory. 
     * Bind this in your WBP_GameInventoryLayout Blueprint (the Exit/Close button).
     */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCommonButtonBase> InventoryExitButton;

private:
    /**
     * Cached reference to the owning player controller.
     * Populated in NativeConstruct() by calling GetOwningPlayer().
     */
    UPROPERTY()
    TObjectPtr<APlayerController> CachedPlayerController;

    /** Called when the exit button is clicked. */
    UFUNCTION()
    void OnInventoryExitButtonClicked();
};
