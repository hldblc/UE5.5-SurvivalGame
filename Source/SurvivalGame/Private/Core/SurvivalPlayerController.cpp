// SurvivalPlayerController.cpp
#include "SurvivalPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "UI/Widgets/MasterUILayout.h"

ASurvivalPlayerController::ASurvivalPlayerController()
{
    RootLayout = nullptr;
}

void ASurvivalPlayerController::BeginPlay()
{
    Super::BeginPlay();

    InitializeEnhancedInput();
    CreateMasterLayout();
}

void ASurvivalPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
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
                UE_LOG(LogTemp, Warning, TEXT("Missing DefaultInputMapping in SurvivalPlayerController"));
            }
        }
    }
}

void ASurvivalPlayerController::CreateMasterLayout()
{
    if (!IsLocalController()) return;
    
    if (MasterLayoutClass && !RootLayout)
    {
        RootLayout = CreateWidget<UMasterUILayout>(this, MasterLayoutClass);
        if (RootLayout)
        {
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
        RootLayout->PushGameInventoryLayout();
        SetInputMode(FInputModeUIOnly());
        bShowMouseCursor = true;
        bInventoryShown = true;
    }
    else
    {
        SetInputMode(FInputModeGameOnly());
        bShowMouseCursor = false;
        bInventoryShown = false;
    }
}