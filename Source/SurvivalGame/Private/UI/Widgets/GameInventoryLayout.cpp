#include "UI/Widgets/GameInventoryLayout.h"
#include "UI/Widgets/InventoryWidget.h"
#include "Components/WidgetSwitcher.h"
#include "CommonButtonBase.h"
#include "SurvivalPlayerController.h"

UGameInventoryLayout::UGameInventoryLayout(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    UE_LOG(LogTemp, Log, TEXT("GameInventoryLayout Constructor"));
}

void UGameInventoryLayout::NativeConstruct()
{
    Super::NativeConstruct();

    UE_LOG(LogTemp, Log, TEXT("GameInventoryLayout::NativeConstruct"));

    // Cache the player controller reference
    CachedPlayerController = Cast<ASurvivalPlayerController>(GetOwningPlayer());

    // If the InventoryExitButton is valid, bind our click event
    if (InventoryExitButton)
    {
        InventoryExitButton->OnClicked().AddUObject(this, &UGameInventoryLayout::OnInventoryExitButtonClicked);
    }


}



void UGameInventoryLayout::OnInventoryExitButtonClicked()
{
    // First try to use cached controller
    ASurvivalPlayerController* PC = CachedPlayerController;
    
    // If cached controller is invalid, try to get it again
    if (!PC)
    {
        PC = Cast<ASurvivalPlayerController>(GetOwningPlayer());
    }

    if (PC)
    {
        IControllerInterface::Execute_CloseInventory(PC);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("OnInventoryExitButtonClicked: No valid PlayerController"));
    }

    // Deactivate the widget
    DeactivateWidget();
}