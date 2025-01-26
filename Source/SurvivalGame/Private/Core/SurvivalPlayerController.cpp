#include "SurvivalPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "UI/Widgets/MasterUILayout.h"

void ASurvivalPlayerController::BeginPlay()
{
	Super::BeginPlay();

	InitializeEnhancedInput();
	CreateMasterLayout();
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

