#include "SurvivalPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/Widgets/MasterUILayout.h"
#include "UI/Widgets/GameInventoryLayout.h" 
#include "Enums/ContainerType.h"
#include "UI/Widgets/Inventory/InventorySlot.h"
#include "UI/Widgets/Inventory/ItemContainerGrid.h"

// Constructor: Kick things off by ensuring our RootLayout pointer is as empty as our coffee cup on Monday morning.
ASurvivalPlayerController::ASurvivalPlayerController()
{
    RootLayout = nullptr;
}

// BeginPlay: The startup party where we set up our input dance moves and create our very own UI stage.
void ASurvivalPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // Task: Initialize enhanced input – because even our UI deserves a snazzy dance routine.
    InitializeEnhancedInput();
    
    // Task: Create our Master UI Layout widget – the ultimate stage for our UI performance.
    CreateMasterLayout();

    
    
}

// EndPlay: Time to clean up after the party—remove the UI stage and put everything away like a pro.
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

// SetupInputComponent: Bind our input actions with style using CommonUI's enhanced system.
// Think of it as teaching your character to groove to the beat.
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

// InitializeEnhancedInput: Configure our fancy input mappings so our UI can strut its stuff.
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

// CreateMasterLayout: Build the main UI stage (Master UI Layout) if it hasn't been set up already.
// This is where all the magic (and quirky UI elements) happens.
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

// InventoryOnClient_Implementation: Toggle the inventory view when our client decides it's time to check out the goods.
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

// CloseInventory_Implementation: Close the inventory view and return control to the game.
// Time to wrap up the performance and go back to business.
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

void ASurvivalPlayerController::InitializeInventoryWidget()
{
    if (!RootLayout)
    {
        UE_LOG(LogTemp, Warning, TEXT("InitializeInventoryWidget: RootLayout is null"));
        return;
    }

    // Push the game inventory layout onto the inventory stack
    UGameInventoryLayout* GameInventoryLayout = RootLayout->PushGameInventoryLayout();
    bInventoryShown = true;

    if (GameInventoryLayout)
    {
        GameInventoryLayout->DeactivateWidget();
        UE_LOG(LogTemp, Log, TEXT("InitializeInventoryWidget: Inventory widget deactivated after push."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("InitializeInventoryWidget: GameInventoryLayout is null"));
    }
    
}

// GetInventoryWidget: Retrieve a specific inventory slot widget based on container type and slot index.
// Think of this as finding the correct seat in our UI auditorium.
// In SurvivalPlayerController.cpp
UInventorySlot* ASurvivalPlayerController::GetInventoryWidget(E_ContainerType ContainerType, int32 SlotIndex)
{
    UE_LOG(LogTemp, Log, TEXT("GetInventoryWidget - CachedLayout: %s"), 
        CachedGameInventoryLayout ? TEXT("Valid") : TEXT("Invalid"));

    if (!CachedGameInventoryLayout)
    {
        if (RootLayout)
        {
            CachedGameInventoryLayout = RootLayout->GetGameInventoryLayout();
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("GetInventoryWidget: Both CachedLayout and RootLayout are invalid!"));
            return nullptr;
        }
    }

    UItemContainerGrid* ContainerGrid = nullptr;
    
    switch (ContainerType)
    {
    case E_ContainerType::Inventory:
        {
            UInventoryWidget* InventoryWidget = CachedGameInventoryLayout->GetInventoryWidget();
            UE_LOG(LogTemp, Log, TEXT("GetInventoryWidget - InventoryWidget: %s"), 
                InventoryWidget ? TEXT("Valid") : TEXT("Invalid"));
            
            if (InventoryWidget)
            {
                ContainerGrid = InventoryWidget->GetItemContainerGrid();
                UE_LOG(LogTemp, Log, TEXT("GetInventoryWidget - ContainerGrid: %s, Slots: %d"), 
                    ContainerGrid ? TEXT("Valid") : TEXT("Invalid"),
                    ContainerGrid ? ContainerGrid->Slots.Num() : 0);
            }
            break;
        }
    default:
        UE_LOG(LogTemp, Warning, TEXT("GetInventoryWidget: Unsupported container type!"));
        return nullptr;
    }

    if (!ContainerGrid)
    {
        UE_LOG(LogTemp, Warning, TEXT("GetInventoryWidget: Container grid not found!"));
        return nullptr;
    }

    // Handle both 0-based and 1-based indices
    int32 ArrayIndex = SlotIndex;
    if (SlotIndex > 0)
    {
        ArrayIndex = SlotIndex - 1;  // Convert 1-based to 0-based if necessary
    }

    if (!ContainerGrid->Slots.IsValidIndex(ArrayIndex))
    {
        UE_LOG(LogTemp, Warning, TEXT("GetInventoryWidget: Invalid slot index %d (Array index: %d)! Total slots: %d"), 
            SlotIndex, ArrayIndex, ContainerGrid->Slots.Num());
        return nullptr;
    }

    UE_LOG(LogTemp, Log, TEXT("GetInventoryWidget: Successfully found slot at index %d (Array index: %d)"), 
        SlotIndex, ArrayIndex);
    return ContainerGrid->Slots[ArrayIndex];
}

// UpdateItemSlot_Implementation: Update an inventory slot by passing the update along to our client function.
// This function is like the secret memo that gets forwarded to the right team member.
void ASurvivalPlayerController::UpdateItemSlot_Implementation(E_ContainerType ContainerType, FItemStructure ItemInfo, int32 Index)
{
    Client_UpdateSlot_Implementation(ContainerType, ItemInfo, Index);
}

// Client_UpdateSlot_Implementation: The client-side method to update an inventory slot.
void ASurvivalPlayerController::Client_UpdateSlot_Implementation(E_ContainerType Container, FItemStructure ItemInfo, int32 Index)
{
    UE_LOG(LogTemp, Log, TEXT("Client_UpdateSlot_Implementation - Container: %d, Index: %d"), 
        (int32)Container, Index);

    UInventorySlot* SlotWidget = GetInventoryWidget(Container, Index);
    
    if (IsValid(SlotWidget))
    {
        SlotWidget->UpdateSlot(ItemInfo);
        UE_LOG(LogTemp, Log, TEXT("Successfully updated slot at index %d"), Index);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Client_UpdateSlot_Implementation: Inventory slot at index %d is missing"), Index);
    }
}


void ASurvivalPlayerController::CacheGameInventoryLayout(UGameInventoryLayout* Layout)
{
    UE_LOG(LogTemp, Log, TEXT("Caching GameInventoryLayout"));
    CachedGameInventoryLayout = Layout;
}