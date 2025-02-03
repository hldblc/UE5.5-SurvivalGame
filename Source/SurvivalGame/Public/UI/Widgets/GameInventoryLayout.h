#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h" // Using CommonUI's activatable widget base.
#include "UI/Widgets/InventoryWidget.h"  // (Optional) Reference if you use a custom InventoryWidget.
#include "GameInventoryLayout.generated.h"

class UWidgetSwitcher;
class UInventoryWidget;
class UCommonButtonBase; // From CommonUI

/**
 * @brief Layout widget managing the game's inventory interface tabs.
 * Blueprint: WBP_GameInventoryLayout (Make sure you use a CommonUI-compatible widget).
 */
UCLASS(BlueprintType, Blueprintable, meta = (DisplayName = "Game Inventory Layout"))
class SURVIVALGAME_API UGameInventoryLayout : public UCommonActivatableWidget
{
    GENERATED_BODY()

public:
    // Constructor using the object initializer.
    UGameInventoryLayout(const FObjectInitializer& ObjectInitializer);

protected:
    // Called when the widget is constructed.
    virtual void NativeConstruct() override;

    /** Reference to the exit button (bind this in your UMG Designer using CommonUI's button class). */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCommonButtonBase> InventoryExitButton;

    /** Handler for when the exit button is clicked. */
    UFUNCTION()
    void OnInventoryExitButtonClicked();
};
