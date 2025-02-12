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
    UE_LOG(LogTemp, Log, TEXT("PushGameInventoryLayout() Called."));
    if (!ensure(GameInventoryStack))
    {
        UE_LOG(LogTemp, Warning, TEXT("GameInventoryStack is nullptr!"));
        return nullptr;
    }
    if (!ensure(GameInventoryLayoutClass))
    {
        UE_LOG(LogTemp, Warning, TEXT("GameInventoryLayoutClass is nullptr! Did you set it in the BP?"));
        return nullptr;
    }

    UCommonActivatableWidget* Widget = GameInventoryStack->AddWidget(GameInventoryLayoutClass);
    if (Widget)
    {
        GameInventoryLayout = Cast<UGameInventoryLayout>(Widget);
        if (GameInventoryLayout)
        {
            UE_LOG(LogTemp, Log, TEXT("PushGameInventoryLayout: Successfully created %s"), *GameInventoryLayout->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("PushGameInventoryLayout: Widget wasn't a UGameInventoryLayout!"));
        }
        return GameInventoryLayout;
    }

    UE_LOG(LogTemp, Warning, TEXT("PushGameInventoryLayout: AddWidget returned nullptr!"));
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
