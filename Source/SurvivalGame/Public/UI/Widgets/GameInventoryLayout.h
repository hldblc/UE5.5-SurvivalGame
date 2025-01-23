#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "UI/Widgets/InventoryWidget.h"
#include "GameInventoryLayout.generated.h"

class UWidgetSwitcher;
class UInventoryWidget;

UENUM(BlueprintType)
enum class EInventoryLayoutTab : uint8
{
    Inventory    UMETA(DisplayName = "Inventory"),
    Engrams      UMETA(DisplayName = "Engrams"),
    Tribe        UMETA(DisplayName = "Tribe"),
    Map          UMETA(DisplayName = "Map")
};

/**
 * @brief Layout widget that manages the game's inventory interface tabs
 * Blueprint: WBP_GameInventoryLayout
 */
UCLASS()
class SURVIVALGAME_API UGameInventoryLayout : public UCommonActivatableWidget
{
    GENERATED_BODY()

public:
    UGameInventoryLayout(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual void NativeOnActivated() override;
    virtual void NativeOnDeactivated() override;

    /** Main widget switcher for tabs */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UWidgetSwitcher> TabSwitcher;

    /** Reference to inventory tab widget (Blueprint: W_InventoryWidget) */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UInventoryWidget> InventoryWidget;

    /** Switch to specified tab */
    UFUNCTION(BlueprintCallable, Category = "UI|Navigation")
    void SwitchToTab(EInventoryLayoutTab NewTab);

    /** Get currently active tab */
    UFUNCTION(BlueprintPure, Category = "UI|State")
    EInventoryLayoutTab GetActiveTab() const { return ActiveTab; }

private:
    /** Currently active tab */
    EInventoryLayoutTab ActiveTab;

    /** Initialize tabs and widget references */
    void InitializeTabs();

    /** Validate required widget bindings */
    bool ValidateWidgetBindings() const;
};