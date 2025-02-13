// GameInventoryLayout.h
#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h" 
#include "UI/Widgets/InventoryWidget.h"  
#include "GameInventoryLayout.generated.h"

class UWidgetSwitcher;
class UInventoryWidget;
class UCommonButtonBase;
class ASurvivalPlayerController;

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

    /** Gets the main inventory widget */
    UInventoryWidget* GetInventoryWidget() const { return InventoryWidget; }

    /** Cache this layout in the player controller */
    void CacheInPlayerController();

protected:
    // Called when the widget is constructed.
    virtual void NativeConstruct() override;

    /** Main inventory widget reference. Bind this in Blueprint. */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UInventoryWidget> InventoryWidget;

    /** Reference to the exit button (bind this in your UMG Designer using CommonUI's button class). */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCommonButtonBase> InventoryExitButton;

    /** Handler for when the exit button is clicked. */
    UFUNCTION()
    void OnInventoryExitButtonClicked();

private:
    /** Cached reference to the owning player controller */
    UPROPERTY()
    TObjectPtr<ASurvivalPlayerController> CachedPlayerController;
};