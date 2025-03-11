#include "UI/Widgets/GameInventoryLayout.h"
#include "CommonButtonBase.h"
#include "SurvivalPlayerController.h"
#include "Interfaces/ControllerInterface.h"
#include "Inventory/ItemContainerBase.h"

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

    if (InventoryWidget)
    {
        // Since the ItemContainerGrid is a widget and not a component,
        // we need to get the owner pawn and look for the inventory component there
        APawn* OwnerPawn = GetOwningPlayerPawn();
        if (OwnerPawn)
        {
            UItemContainerBase* InventoryContainer = Cast<UItemContainerBase>(
                OwnerPawn->FindComponentByClass(UItemContainerBase::StaticClass()));
            
            if (InventoryContainer)
            {
                InventoryContainer->PrintInventoryContents();
                UE_LOG(LogTemp, Log, TEXT("Printed inventory contents for debugging"));
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Could not find inventory container component"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Could not get owning player pawn"));
        }
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

    
    if (CachedPlayerController)
    {
        
        IControllerInterface::Execute_CloseInventory(CachedPlayerController);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("NativeOnDeactivated: No valid PlayerController to close inventory."));
    }
}