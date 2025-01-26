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
    if (!ensure(GameHUDStack && DefaultHUDLayoutClass))
    {
        return nullptr;
    }

    if (UCommonActivatableWidget* Widget = GameHUDStack->AddWidget(DefaultHUDLayoutClass))
    {
        DefaultHUDLayout = Cast<UDefaultHUDLayout>(Widget);
        return DefaultHUDLayout;
    }

    return nullptr;
}


UGameInventoryLayout* UMasterUILayout::PushGameInventoryLayout()
{
    if (!ensure(GameInventoryStack && GameInventoryLayoutClass))
    {
        return nullptr;
    }

    if (UCommonActivatableWidget* Widget = GameInventoryStack->AddWidget(GameInventoryLayoutClass))
    {
        GameInventoryLayout = Cast<UGameInventoryLayout>(Widget);
        return GameInventoryLayout;
    }

    return nullptr;
}