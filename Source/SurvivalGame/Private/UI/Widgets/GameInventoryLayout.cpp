#include "UI/Widgets/GameInventoryLayout.h"
#include "UI/Widgets/InventoryWidget.h"
#include "Components/WidgetSwitcher.h"

UGameInventoryLayout::UGameInventoryLayout(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , ActiveTab(EInventoryLayoutTab::Inventory)
{
}

void UGameInventoryLayout::NativeConstruct()
{
    Super::NativeConstruct();

    if (ValidateWidgetBindings())
    {
        InitializeTabs();
    }
}

void UGameInventoryLayout::NativeDestruct()
{
    Super::NativeDestruct();
}

void UGameInventoryLayout::NativeOnActivated()
{
    Super::NativeOnActivated();

    // Switch to inventory tab by default when activated
    SwitchToTab(EInventoryLayoutTab::Inventory);
}

void UGameInventoryLayout::NativeOnDeactivated()
{
    Super::NativeOnDeactivated();
}

void UGameInventoryLayout::InitializeTabs()
{
    // Initial setup for tabs
    SwitchToTab(EInventoryLayoutTab::Inventory);
}

void UGameInventoryLayout::SwitchToTab(EInventoryLayoutTab NewTab)
{
    if (!ensure(TabSwitcher))
    {
        return;
    }

    // Set the switcher to the appropriate index based on the tab
    switch (NewTab)
    {
        case EInventoryLayoutTab::Inventory:
            TabSwitcher->SetActiveWidgetIndex(0);
            break;
        case EInventoryLayoutTab::Engrams:
            TabSwitcher->SetActiveWidgetIndex(1);
            break;
        case EInventoryLayoutTab::Tribe:
            TabSwitcher->SetActiveWidgetIndex(2);
            break;
        case EInventoryLayoutTab::Map:
            TabSwitcher->SetActiveWidgetIndex(3);
            break;
    }

    ActiveTab = NewTab;
}

bool UGameInventoryLayout::ValidateWidgetBindings() const
{
    bool bIsValid = true;

    if (!TabSwitcher)
    {
        UE_LOG(LogTemp, Error, TEXT("%s: TabSwitcher is not bound in widget Blueprint!"), *GetName());
        bIsValid = false;
    }

    if (!InventoryWidget)
    {
        UE_LOG(LogTemp, Error, TEXT("%s: InventoryWidget (W_InventoryWidget in Blueprint) is not bound!"), *GetName());
        bIsValid = false;
    }

    return bIsValid;
}