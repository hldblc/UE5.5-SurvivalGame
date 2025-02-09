#include "SurvivalPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "UI/Widgets/MasterUILayout.h"
#include "UI/Widgets/Inventory/InventorySlot.h"

ASurvivalPlayerController::ASurvivalPlayerController()
{
    RootLayout = nullptr;
}

void ASurvivalPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // Initialize our enhanced input (CommonUI loves a good input mapping).
    InitializeEnhancedInput();
    
    // Create and add our Master UI Layout widget (the stage for our CommonUI performance).
    CreateMasterLayout();
}

void ASurvivalPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Remove the Master UI Layout from the viewport if it's still there.
    if (RootLayout)
    {
        RootLayout->RemoveFromParent();
        RootLayout = nullptr;
    }
    
    Super::EndPlay(EndPlayReason);
}

void ASurvivalPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    // Bind our inventory toggle action using CommonUI's enhanced input system.
    if (UEnhancedInputComponent* EnhancedInput = CastChecked<UEnhancedInputComponent>(InputComponent))
    {
        if (ensure(IA_InventoryToggle))
        {
            EnhancedInput->BindAction(IA_InventoryToggle, ETriggerEvent::Started, 
                this, &ASurvivalPlayerController::InventoryOnClient);
        }
    }
}

void ASurvivalPlayerController::InitializeEnhancedInput()
{
    // Get our local player and add the default input mapping from CommonUI.
    if (const ULocalPlayer* LocalPlayer = GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            if (DefaultInputMapping != nullptr)
            {
                Subsystem->AddMappingContext(DefaultInputMapping, 0);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Missing DefaultInputMapping in SurvivalPlayerController - CommonUI mode activated!"));
            }
        }
    }
}

void ASurvivalPlayerController::CreateMasterLayout()
{
    // Only create the layout for local controllers (CommonUI exclusive!).
    if (!IsLocalController()) return;
    
    // Create the Master UI Layout widget if it hasn't been created yet.
    if (MasterLayoutClass && !RootLayout)
    {
        RootLayout = CreateWidget<UMasterUILayout>(this, MasterLayoutClass);
        if (RootLayout)
        {
            // Add our Master UI Layout to the viewport and push the default HUD layout.
            RootLayout->AddToViewport();
            RootLayout->PushDefaultHUDLayout();
        }
    }
}

void ASurvivalPlayerController::InventoryOnClient_Implementation()
{
    if (!RootLayout) return;
    
    if (!bInventoryShown)
    {
        // Open the inventory by pushing the inventory widget onto the CommonUI stack.
        RootLayout->PushGameInventoryLayout();
        
        // Change the input mode to UI only and show the mouse cursor.
        SetInputMode(FInputModeUIOnly());
        bShowMouseCursor = true;
        bInventoryShown = true;
    }
    else
    {
        // If the inventory is open and the toggle key is pressed, close it.
        CloseInventory_Implementation();
    }
}

void ASurvivalPlayerController::CloseInventory_Implementation()
{
    // Call the Master UI Layout to pop (remove) the inventory widget.
    if (RootLayout)
    {
        RootLayout->PopGameInventoryLayout();
    }
    
    // Reset the input mode to game only and hide the mouse cursor.
    SetInputMode(FInputModeGameOnly());
    bShowMouseCursor = false;
    bInventoryShown = false;
}

void ASurvivalPlayerController::GetInventoryWidget(E_ContainerType ContainerType, int32 SlotIndex)
{
    
}



