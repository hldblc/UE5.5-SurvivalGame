#include "SurvivalPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/Widgets/MasterUILayout.h"
#include "UI/Widgets/GameInventoryLayout.h"
#include "UI/Widgets/InventoryWidget.h"
#include "Enums/ContainerType.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Widgets/Inventory/InventorySlot.h"
#include "UI/Widgets/Inventory/ItemContainerGrid.h"

//==================================================Constructor==================================================
ASurvivalPlayerController::ASurvivalPlayerController()
{
    RootLayout = nullptr;
}

//==================================================BeginPlay==================================================
void ASurvivalPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // Task: Initialize enhanced input – because even our UI deserves a snazzy dance routine.
    InitializeEnhancedInput();
    
    // Task: Create our Master UI Layout widget – the ultimate stage for our UI performance.
    CreateMasterLayout();

    
    
}

//==================================================EndPlay==================================================
void ASurvivalPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (RootLayout)
    {
        if (RootLayout->GetParent())
        {
            RootLayout->RemoveFromParent();
        }
        RootLayout = nullptr;
    }
    
    Super::EndPlay(EndPlayReason);
}

//==================================================SetupInputComponent==================================================
void ASurvivalPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    // Task: Convert our input component into an EnhancedInputComponent (because regular inputs are so last season).
    if (UEnhancedInputComponent* EnhancedInput = CastChecked<UEnhancedInputComponent>(InputComponent))
    {
        // Task: Double-check that our inventory toggle action is set up, then bind it like a boss.
        if (ensure(IA_InventoryToggle))
        {
            EnhancedInput->BindAction(IA_InventoryToggle, ETriggerEvent::Started, 
                this, &ASurvivalPlayerController::InventoryOnClient);
        }
    }
}

//==================================================InitializeEnhancedInput==================================================
void ASurvivalPlayerController::InitializeEnhancedInput() const
{
    // Task: Get our local player and add the default input mapping – think of it as handing out VIP passes.
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

//==================================================CreateMasterLayout==================================================
void ASurvivalPlayerController::CreateMasterLayout()
{
    // Task: Only create the stage for local controllers (because remote ones aren’t invited to this party).
    if (!IsLocalController()) return;
    
    // Task: Create our Master UI Layout widget if it doesn’t exist – we wouldn’t want an empty stage now, would we?
    if (MasterLayoutClass && !RootLayout)
    {
        RootLayout = CreateWidget<UMasterUILayout>(this, MasterLayoutClass);
        if (RootLayout)
        {
            // Task: Add our stage to the viewport and kick off the default HUD layout.
            RootLayout->AddToViewport();
            RootLayout->PushDefaultHUDLayout();
            
        }
    }
}



//==================================================InventoryOnClient==================================================
void ASurvivalPlayerController::InventoryOnClient_Implementation()
{
    UE_LOG(LogTemp, Log, TEXT("InventoryOnClient_Implementation - Starting"));
    
    if (!RootLayout)
    {
        UE_LOG(LogTemp, Warning, TEXT("InventoryOnClient_Implementation: RootLayout is null"));
        return;
    }
    
    if (!bInventoryShown)
    {
        UE_LOG(LogTemp, Log, TEXT("InventoryOnClient_Implementation - Opening inventory"));
        
        UGameInventoryLayout* InvLayout = RootLayout->PushGameInventoryLayout();
        if (InvLayout)
        {
            UE_LOG(LogTemp, Log, TEXT("InventoryOnClient_Implementation - GameInventoryLayout pushed successfully"));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("InventoryOnClient_Implementation - Failed to push GameInventoryLayout"));
        }
        
        SetInputMode(FInputModeUIOnly());
        bShowMouseCursor = true;
        bInventoryShown = true;

        if (APawn* LocalPawn = GetPawn())
        {
            if (ACharacter* LocalCharacter = Cast<ACharacter>(LocalPawn))
            {
                LocalCharacter->GetCharacterMovement()->DisableMovement();
            }
        }
    }
    else
    {
        CloseInventory_Implementation();
    }
}



//==================================================CloseInventory==================================================
void ASurvivalPlayerController::CloseInventory_Implementation()
{
    // Task: Tell our Master UI Layout to remove the inventory widget from the stage.
    if (RootLayout)
    {
        RootLayout->PopGameInventoryLayout();
    }
    
    // Task: Switch back to game mode, hide the cursor, and restore our focus to the game.
    SetInputMode(FInputModeGameOnly());
    bShowMouseCursor = false;
    bInventoryShown = false;
    if (APawn* LocalPawn = GetPawn())
    {
        if (ACharacter* LocalCharacter = Cast<ACharacter>(LocalPawn))
        {
            LocalCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
        }
    }
}



//==================================================
void ASurvivalPlayerController::InitializeInventoryWidget()
{
    if (!RootLayout)
    {
        UE_LOG(LogTemp, Warning, TEXT("InitializeInventoryWidget: RootLayout is null"));
        return; 
    }

    UGameInventoryLayout* GameInventoryLayout = RootLayout->PushGameInventoryLayout();
    bInventoryShown = true;

    if (GameInventoryLayout)
    {
        GameInventoryLayout->DeactivateWidget();
        UE_LOG(LogTemp, Log, TEXT("InitializeInventoryWidget: GameInventoryLayout pushed successfully"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("InitializeInventoryWidget: Failed to push GameInventoryLayout"));
    }
}



//==================================================GetInventorySlotWidget==================================================
UInventorySlot* ASurvivalPlayerController::GetInventorySlotWidget(E_ContainerType ContainerType, int32 SlotIndex)
{
    // 1. Check if the main UI container (RootLayout) is available.
    if (!RootLayout)
    {
        UE_LOG(LogTemp, Warning, TEXT("GetInventoryWidget: RootLayout is null"));
        return nullptr;
    }

    // 2. If inventory isn't shown, temporarily show it to initialize the widgets
    bool bWasInventoryHidden = !bInventoryShown;
    if (bWasInventoryHidden)
    {
        UGameInventoryLayout* InvLayout = RootLayout->PushGameInventoryLayout();
        if (!InvLayout)
        {
            UE_LOG(LogTemp, Warning, TEXT("GetInventoryWidget: Failed to create GameInventoryLayout"));
            return nullptr;
        }
    }

    // 3. Now try to get the layout
    UGameInventoryLayout* GameInventoryLayout = RootLayout->GetGameInventoryLayout();
    if (!GameInventoryLayout)
    {
        UE_LOG(LogTemp, Warning, TEXT("GetInventoryWidget: GameInventoryLayout is null"));
        return nullptr;
    }

    UInventoryWidget* InventoryWidget = GameInventoryLayout->GetInventoryWidget();
    if (!InventoryWidget)
    {
        UE_LOG(LogTemp, Warning, TEXT("GetInventoryWidget: InventoryWidget is null"));
        return nullptr;
    }

    UItemContainerGrid* ItemContainerGrid = InventoryWidget->GetItemContainerGrid();
    if (!ItemContainerGrid)
    {
        UE_LOG(LogTemp, Warning, TEXT("GetInventoryWidget: ItemContainerGrid is null"));
        return nullptr;
    }

    if (!ItemContainerGrid->Slots.IsValidIndex(SlotIndex))
    {
        UE_LOG(LogTemp, Warning, TEXT("GetInventoryWidget: Invalid slot index"));
        return nullptr;
    }

    // 4. If we temporarily showed the inventory, hide it again
    if (bWasInventoryHidden && GameInventoryLayout)
    {
        GameInventoryLayout->DeactivateWidget();
    }

    // 5. Return the appropriate slot
    switch (ContainerType)
    {
        case E_ContainerType::Inventory:
            return ItemContainerGrid->Slots[SlotIndex];
        default:
            return ItemContainerGrid->Slots[SlotIndex];
    }
}



//==================================================UpdateItemSlot==================================================
void ASurvivalPlayerController::UpdateItemSlot_Implementation(E_ContainerType ContainerType, FItemStructure ItemInfo, int32 Index)
{
    Client_UpdateSlot_Implementation(ContainerType, ItemInfo, Index);
}



//==================================================Client_UpdateSlot==================================================
void ASurvivalPlayerController::Client_UpdateSlot_Implementation(E_ContainerType Container, FItemStructure ItemInfo, int32 Index)
{
    UInventorySlot* InventorSlot = GetInventorySlotWidget(Container, Index);

    if (IsValid(InventorSlot))
    {
        // Update the slot with the new item info
        InventorSlot->UpdateSlot(ItemInfo);
    }

}

