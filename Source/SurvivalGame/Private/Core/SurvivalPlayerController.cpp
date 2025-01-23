#include "Core/SurvivalPlayerController.h"
#include "EnhancedInputComponent.h"
#include "UI/Widgets/MasterUILayout.h"
#include "Net/UnrealNetwork.h"

ASurvivalPlayerController::ASurvivalPlayerController()
    : bInventoryShown(false)
{
}

void ASurvivalPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ASurvivalPlayerController, bInventoryShown);
}

void ASurvivalPlayerController::BeginPlay()
{
    Super::BeginPlay();
}

void ASurvivalPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
    {
        EnhancedInputComponent->BindAction(IA_InventoryToggle, ETriggerEvent::Started, 
            this, &ASurvivalPlayerController::HandleInventoryToggle);
    }
}

void ASurvivalPlayerController::HandleInventoryToggle(const FInputActionInstance& Instance)
{
    InventoryOnClient();
}

void ASurvivalPlayerController::InventoryOnClient_Implementation()
{
    // Add delay as per SmartPoly's implementation
    FTimerHandle DelayHandle;
    GetWorld()->GetTimerManager().SetTimer(DelayHandle, [this]()
    {
        if (!bInventoryShown)
        {
            if (RootLayout)
            {
                RootLayout->PushGameInventoryLayout();
                UpdateInputMode(true);
                SetMouseCursorVisibility(true);
                bInventoryShown = true;
            }
        }
        else
        {
            UpdateInputMode(false);
            SetMouseCursorVisibility(false);
            bInventoryShown = false;
        }
    }, 0.1f, false);
}

void ASurvivalPlayerController::UpdateInputMode(bool bShowUI)
{
    if (bShowUI)
    {
        SetInputMode(FInputModeUIOnly());
    }
    else
    {
        SetInputMode(FInputModeGameOnly());
    }
}

void ASurvivalPlayerController::SetMouseCursorVisibility(bool bShow)
{
    bShowMouseCursor = bShow;
}