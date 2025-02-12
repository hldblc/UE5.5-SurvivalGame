#include "SurvivalPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/Widgets/MasterUILayout.h"
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
    // Task: If our UI stage (RootLayout) is still up, kick it out of the viewport.
    if (RootLayout)
    {
        RootLayout->RemoveFromParent();
        RootLayout = nullptr; // Reset our pointer so it doesn’t hang around like last week’s memes.
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
void ASurvivalPlayerController::InitializeEnhancedInput()
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
    if (!RootLayout) return;
    
    // Task: If the inventory is currently shy (not shown), then show it off!
    if (!bInventoryShown)
    {
        // Open the inventory by adding the inventory widget to our UI stack.
        RootLayout->PushGameInventoryLayout();
        
        // Task: Switch the input mode to UI-only, because sometimes you just need to click around.
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
        // Task: If the inventory is already strutting its stuff, then politely ask it to exit stage left.
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

// GetInventoryWidget: Retrieve a specific inventory slot widget based on container type and slot index.
// Think of this as finding the correct seat in our UI auditorium.
UInventorySlot* ASurvivalPlayerController::GetInventoryWidget(E_ContainerType ContainerType, int32 SlotIndex)
{
    // Task: Make sure our main UI stage (RootLayout) is present; if not, log a complaint and bail out.
    if (!RootLayout)
    {
        UE_LOG(LogTemp, Warning, TEXT("GetInventoryWidget: RootLayout is null"));
        return nullptr;
    }

    // Task: Fetch the Inventory Command Center (Game Inventory Layout) from the Master UI Layout.
    UGameInventoryLayout* GameInventoryLayout = RootLayout->GetGameInventoryLayout();
    if (!GameInventoryLayout)
    {
        UE_LOG(LogTemp, Warning, TEXT("GetInventoryWidget: GameInventoryLayout is null"));
        return nullptr;
    }
    
    // Task: Locate the Inventory Panel within the Game Inventory Layout.
    // We search for a child widget named "InventoryWidget" – set this name in your UMG if you haven’t already.
    UInventoryWidget* InventoryWidget = Cast<UInventoryWidget>(GameInventoryLayout->GetWidgetFromName(TEXT("InventoryWidget")));
    if (!InventoryWidget)
    {
        UE_LOG(LogTemp, Warning, TEXT("GetInventoryWidget: InventoryWidget is null"));
        return nullptr;
    }
    
    // Task: Find the ItemContainerGrid within the Inventory Panel.
    // This grid holds our array of inventory slot widgets – our very own seating chart!
    UItemContainerGrid* ContainerGrid = Cast<UItemContainerGrid>(InventoryWidget->GetWidgetFromName(TEXT("ItemContainerGrid")));
    if (!ContainerGrid)
    {
        UE_LOG(LogTemp, Warning, TEXT("GetInventoryWidget: ItemContainerGrid is null"));
        return nullptr;
    }
    
    // Task: Check that the provided SlotIndex is within the valid range of our seating chart.
    if (!ContainerGrid->Slots.IsValidIndex(SlotIndex))
    {
        UE_LOG(LogTemp, Warning, TEXT("GetInventoryWidget: SlotIndex %d is invalid"), SlotIndex);
        return nullptr;
    }
    
    // Task: Decide which inventory slot to return based on the container type.
    // Currently, only the 'Inventory' type is handled – our code is ready to party with more types in the future.
    switch (ContainerType)
    {
        case E_ContainerType::Inventory:
            return ContainerGrid->Slots[SlotIndex];
        default:
            return ContainerGrid->Slots[SlotIndex];
    }
}

// UpdateItemSlot_Implementation: Update an inventory slot by passing the update along to our client function.
// This function is like the secret memo that gets forwarded to the right team member.
void ASurvivalPlayerController::UpdateItemSlot_Implementation(E_ContainerType ContainerType, FItemStructure ItemInfo, int32 Index)
{
    return Client_UpdateSlot_Implementation(ContainerType, ItemInfo, Index);
}

// Client_UpdateSlot_Implementation: The client-side method to update an inventory slot.
void ASurvivalPlayerController::Client_UpdateSlot_Implementation(E_ContainerType Container, FItemStructure ItemInfo, int32 Index)
{
    // Step 1: Let's hunt down our inventory widget.
    // Missing widgets are like that one friend who never RSVPs—totally not cool.
    UInventorySlot* SlotWidget = GetInventoryWidget(Container, Index);
    
    // Step 2: Check if we actually got the widget.
    // Null pointers are like ghosting in real life—nobody likes them.
    if (IsValid(SlotWidget))
    {
        // Step 3: Give the widget a fresh update.
        // It's time to make it glow like your favorite new sneakers.
        SlotWidget->UpdateSlot(ItemInfo);
    }
    else
    {
        // If the widget didn't show up, log a warning.
        // Consider this a friendly heads-up that our widget might be lost in the code abyss.
        UE_LOG(LogTemp, Warning, TEXT("Client_UpdateSlot_Implementation: Inventory slot at index %d is missing. Did it screw us?"), Index);
    }
}
