#include "UI/Widgets/MasterUILayout.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "UI/Widgets/GameInventoryLayout.h"

UMasterUILayout::UMasterUILayout(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

UGameInventoryLayout* UMasterUILayout::PushGameInventoryLayout()
{
    // Only proceed if we have valid references
    if (!ensure(GameInventoryStack && GameInventoryLayoutClass))
    {
        return nullptr;
    }

    // Add the game inventory layout to the stack
    if (UCommonActivatableWidget* Widget = GameInventoryStack->AddWidget(GameInventoryLayoutClass))
    {
        GameInventoryLayout = Cast<UGameInventoryLayout>(Widget);
        return GameInventoryLayout;
    }

    return nullptr;
}