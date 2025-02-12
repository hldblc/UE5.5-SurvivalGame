#include "UI/Widgets/MasterUILayout.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "UI/Widgets/GameInventoryLayout.h"
#include "UI/Widgets/DefaultHUDLayout.h"

UMasterUILayout::UMasterUILayout(const FObjectInitializer& ObjectInitializer)
   : Super(ObjectInitializer)
{
}

UDefaultHUDLayout* UMasterUILayout::PushDefaultHUDLayout()
{
    // Ensure the HUD container and the default HUD layout class are valid.
    if (!ensure(GameHUDStack && DefaultHUDLayoutClass))
    {
        return nullptr;
    }

    // Add the default HUD layout widget to the HUD stack (CommonUI style!).
    if (UCommonActivatableWidget* Widget = GameHUDStack->AddWidget(DefaultHUDLayoutClass))
    {
        DefaultHUDLayout = Cast<UDefaultHUDLayout>(Widget);
        return DefaultHUDLayout;
    }

    return nullptr;
}

UGameInventoryLayout* UMasterUILayout::PushGameInventoryLayout()
{
    // Ensure that our inventory container and the inventory layout class are valid.
    if (!ensure(GameInventoryStack && GameInventoryLayoutClass))
    {
        return nullptr;
    }

    // Add the inventory widget to the correct inventory stack.
    if (UCommonActivatableWidget* Widget = GameInventoryStack->AddWidget(GameInventoryLayoutClass))
    {
        GameInventoryLayout = Cast<UGameInventoryLayout>(Widget);
        return GameInventoryLayout;
    }
    return nullptr;
}

void UMasterUILayout::PopGameInventoryLayout()
{
    // If our inventory widget is still around, remove it from its parent and set our pointer to nullptr.
    if (GameInventoryLayout)
    {
        GameInventoryLayout->RemoveFromParent();
        GameInventoryLayout = nullptr;
    }
}
