#include "SurvivalPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "UI/Widgets/MasterUILayout.h"

void ASurvivalPlayerController::BeginPlay()
{
	Super::BeginPlay();

	InitializeEnhancedInput();
	CreateMasterLayout();
}

void ASurvivalPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInput = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInput->BindAction(IA_InventoryToggle, ETriggerEvent::Started, 
			this, &ASurvivalPlayerController::InventoryOnClient);
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


void ASurvivalPlayerController::InitializeEnhancedInput()
{
	// Get Enhanced Input Subsystem
	if (const ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			// Check if input mapping is valid
			if (DefaultInputMapping != nullptr) // Direct pointer check
			{
				// Add mapping context with priority 0
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
			RootLayout->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			RootLayout->PushDefaultHUDLayout();
			
		}
	}
}

