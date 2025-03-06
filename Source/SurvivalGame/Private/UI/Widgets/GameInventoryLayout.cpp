#include "UI/Widgets/GameInventoryLayout.h"
#include "CommonButtonBase.h"
#include "SurvivalPlayerController.h"
#include "Interfaces/ControllerInterface.h"

UGameInventoryLayout::UGameInventoryLayout(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    UE_LOG(LogTemp, Log, TEXT("GameInventoryLayout Constructor"));
}

void UGameInventoryLayout::NativeConstruct()
{
    Super::NativeConstruct();

    UE_LOG(LogTemp, Log, TEXT("GameInventoryLayout::NativeConstruct"));

    // Cache the owning player controller for later use
    CachedPlayerController = Cast<APlayerController>(GetOwningPlayer());

    // Bind the exit button's OnClicked event
    if (InventoryExitButton)
    {
        InventoryExitButton->OnClicked().AddUObject(this, &UGameInventoryLayout::OnInventoryExitButtonClicked);
    }
}

void UGameInventoryLayout::OnInventoryExitButtonClicked()
{
    // In Blueprint, we used to call "DeactivateWidget()" on the exit button.
    // That triggers NativeOnDeactivated() below.
    DeactivateWidget();
}

void UGameInventoryLayout::NativeOnDeactivated()
{
    Super::NativeOnDeactivated();

    // In Blueprint, "Event On Deactivated" -> "IsValid?" -> "Close Inventory".
    // We do the same here via C++:
    if (CachedPlayerController)
    {
        // Because SurvivalPlayerController implements IControllerInterface, 
        // we can call CloseInventory via Execute_CloseInventory().
        IControllerInterface::Execute_CloseInventory(CachedPlayerController);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("NativeOnDeactivated: No valid PlayerController to close inventory."));
    }
}
