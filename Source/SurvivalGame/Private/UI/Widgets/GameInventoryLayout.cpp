#include "UI/Widgets/GameInventoryLayout.h"
#include "UI/Widgets/InventoryWidget.h"
#include "Components/WidgetSwitcher.h"
#include "CommonButtonBase.h" // CommonUI's button base class

UGameInventoryLayout::UGameInventoryLayout(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void UGameInventoryLayout::NativeConstruct()
{
    Super::NativeConstruct();

    // If the InventoryExitButton is valid, bind our click event.
    if (InventoryExitButton)
    {
        // Bind to the CommonUI OnClicked() delegate.
        InventoryExitButton->OnClicked().AddUObject(this, &UGameInventoryLayout::OnInventoryExitButtonClicked);
    }
}

void UGameInventoryLayout::OnInventoryExitButtonClicked()
{
    // When the exit button is clicked, we trigger the widget deactivation flow.
    // This typically fires an OnDeactivated event in Blueprint, where you can handle inventory closure.
    // Consider it like hitting the "escape" key on your inventory—time to pack it up!
    DeactivateWidget();
}
