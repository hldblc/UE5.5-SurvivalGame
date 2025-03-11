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

//==================================================Debug Functions==================================================
void ASurvivalPlayerController::DebugListAllItemAssets()
{
    UAssetManager* AssetManager = UAssetManager::GetIfInitialized();
    if (!AssetManager)
    {
        UE_LOG(LogTemp, Error, TEXT("AssetManager not initialized!"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("=== ALL AVAILABLE ITEM ASSETS ==="));

    // Try to list all UItemInfo assets no matter where they are
    UObjectLibrary* ItemLibrary = UObjectLibrary::CreateLibrary(UItemInfo::StaticClass(), true, true);
    ItemLibrary->LoadAssetDataFromPath("/Game");

    TArray<FAssetData> AllItems;
    ItemLibrary->GetAssetDataList(AllItems);

    UE_LOG(LogTemp, Log, TEXT("ObjectLibrary found %d UItemInfo assets:"), AllItems.Num());
    for (const FAssetData& Asset : AllItems)
    {
        UE_LOG(LogTemp, Log, TEXT("  Path: %s"), *Asset.GetSoftObjectPath().ToString());
    
        // Try to load it and check its registry key
        UItemInfo* Item = Cast<UItemInfo>(Asset.GetAsset());
        if (Item)
        {
            UE_LOG(LogTemp, Log, TEXT("    RegistryKey: %s"), *Item->RegistryKey.ToString());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("    Failed to load asset"));
        }
    }
}

//==================================================BeginPlay==================================================
void ASurvivalPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // 1) Check if we are on the server (HasAuthority()) vs. client
    if (HasAuthority())
    {
        UE_LOG(LogTemp, Log, TEXT("BeginPlay: Running on server (could be dedicated or listen)."));
        // If you have server-only logic (replication setup, etc.), put it here.
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("BeginPlay: Running on a remote client."));
    }

    // Initialize enhanced input
    InitializeEnhancedInput();
    
    // Create our Master UI Layout widget only for local controllers
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
    // Only setup input for local controllers
    if (!IsLocalController())
    {
        return;
    }

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
    // 2) Check if we have a local player controller (listen server or normal client)
    //    If this is a dedicated server with no local player, this will be false, skipping UI creation
    if (!IsLocalController())
    {
        UE_LOG(LogTemp, Log, TEXT("CreateMasterLayout: Not a local controller, skipping UI creation."));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("CreateMasterLayout: Local player controller => creating UI."));
    
    // Create our Master UI Layout widget if it doesn't exist and class is valid
    if (MasterLayoutClass && !RootLayout)
    {
        RootLayout = CreateWidget<UMasterUILayout>(this, MasterLayoutClass);
        if (RootLayout)
        {
            // Add our stage to the viewport and kick off the default HUD layout
            RootLayout->AddToViewport();
            RootLayout->PushDefaultHUDLayout();
            
            UE_LOG(LogTemp, Log, TEXT("CreateMasterLayout: UI widget successfully added to viewport."));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("CreateMasterLayout: Failed to create MasterUILayout widget"));
        }
    }
    else if (!MasterLayoutClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("CreateMasterLayout: MasterLayoutClass is not set"));
    }
}

//==================================================InventoryOnClient==================================================
void ASurvivalPlayerController::InventoryOnClient_Implementation()
{
    // This function only executes on clients
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
    // This interface implementation will be called on both server and clients
    
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

//==================================================InitializeInventoryWidget==================================================
void ASurvivalPlayerController::InitializeInventoryWidget()
{
    // Only perform UI operations on local controllers
    if (!IsLocalController())
    {
        return;
    }
    
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
    // This function should only be called on local controllers
    if (!IsLocalController())
    {
        UE_LOG(LogTemp, Warning, TEXT("GetInventorySlotWidget: Not a local controller"));
        return nullptr;
    }
    
    // 1. Check if the main UI container (RootLayout) is available.
    if (!RootLayout)
    {
        UE_LOG(LogTemp, Warning, TEXT("GetInventorySlotWidget: RootLayout is null"));
        return nullptr;
    }

    // 2. If inventory isn't shown, temporarily show it to initialize the widgets
    bool bWasInventoryHidden = !bInventoryShown;
    if (bWasInventoryHidden)
    {
        UGameInventoryLayout* InvLayout = RootLayout->PushGameInventoryLayout();
        if (!InvLayout)
        {
            UE_LOG(LogTemp, Warning, TEXT("GetInventorySlotWidget: Failed to create GameInventoryLayout"));
            return nullptr;
        }
    }

    // 3. Now try to get the layout
    UGameInventoryLayout* GameInventoryLayout = RootLayout->GetGameInventoryLayout();
    if (!GameInventoryLayout)
    {
        UE_LOG(LogTemp, Warning, TEXT("GetInventorySlotWidget: GameInventoryLayout is null"));
        return nullptr;
    }

    UInventoryWidget* InventoryWidget = GameInventoryLayout->GetInventoryWidget();
    if (!InventoryWidget)
    {
        UE_LOG(LogTemp, Warning, TEXT("GetInventorySlotWidget: InventoryWidget is null"));
        return nullptr;
    }

    UItemContainerGrid* ItemContainerGrid = InventoryWidget->GetItemContainerGrid();
    if (!ItemContainerGrid)
    {
        UE_LOG(LogTemp, Warning, TEXT("GetInventorySlotWidget: ItemContainerGrid is null"));
        return nullptr;
    }

    if (!ItemContainerGrid->Slots.IsValidIndex(SlotIndex))
    {
        UE_LOG(LogTemp, Warning, TEXT("GetInventorySlotWidget: Invalid slot index"));
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

//==================================================UpdateItemSlot Interface==================================================
void ASurvivalPlayerController::UpdateItemSlot_Implementation(E_ContainerType ContainerType, FItemStructure ItemInfo, int32 Index)
{
    // Interface implementations are called on both server and clients
    
    // We need to update the UI on the client that owns this controller
    if (IsLocalController())
    {
        Client_UpdateSlot(ContainerType, ItemInfo, Index);
    }
    else
    {
        // For remote controllers, we need the reliable RPC to update their clients
        Client_UpdateSlot(ContainerType, ItemInfo, Index);
    }
}



//==================================================Client_UpdateSlot==================================================
void ASurvivalPlayerController::Client_UpdateSlot_Implementation(E_ContainerType Container, FItemStructure ItemInfo, int32 Index)
{
    // This RPC only executes on the owning client
    UInventorySlot* InventorSlot = GetInventorySlotWidget(Container, Index);

    if (IsValid(InventorSlot))
    {
        // Update the slot with the new item info
        InventorSlot->UpdateSlot(ItemInfo);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Client_UpdateSlot: Invalid inventory slot at index %d"), Index);
    }
}


// ==================================================ResetItemSlot Interface==================================================
void ASurvivalPlayerController::ResetItemSlot_Implementation(E_ContainerType ContainerType, int32 Index)
{
    Client_ResetSlot(ContainerType, Index);
    
}

// ==================================================Client_ResetSlot==================================================
void ASurvivalPlayerController::Client_ResetSlot_Implementation(E_ContainerType Container, int32 Index)
{
    // Validate index is in reasonable range
    if (Index < 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Client_ResetSlot: Invalid negative index %d"), Index);
        return;
    }
    
    UInventorySlot* InventorySlot = GetInventorySlotWidget(Container, Index);

    if (IsValid(InventorySlot))
    {
        InventorySlot->ClearSlot();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Client_ResetSlot: Failed to get valid inventory slot at index %d"), Index);
    }
}


























