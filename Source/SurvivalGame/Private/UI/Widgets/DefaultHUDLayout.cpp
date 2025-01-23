// DefaultHUDLayout.cpp
#include "UI/Widgets/DefaultHUDLayout.h"

UDefaultHUDLayout::UDefaultHUDLayout(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , OwningPC(nullptr)
    , CurrentHUDScale(1.0f)
{
    // Disable activation focus to ensure player controls work with HUD visible
    bSupportsActivationFocus = false;
    SetIsFocusable(false);

    // Validate scale range configuration
    ensureAlwaysMsgf(ValidateHUDScaleRange(), 
        TEXT("%s: Invalid HUD scale configuration detected in constructor"), *GetNameSafe(this));
}

void UDefaultHUDLayout::NativeConstruct()
{
    Super::NativeConstruct();

    // Cache player controller reference
    OwningPC = GetOwningPlayer();
    
    // Provide fallback for missing player controller
    if (!ensureMsgf(OwningPC, TEXT("%s: Failed to get owning player controller!"), *GetNameSafe(this)))
    {
        // Attempt to get world context object's player controller as fallback
        if (UWorld* World = GetWorld())
        {
            OwningPC = World->GetFirstPlayerController();
            UE_LOG(LogTemp, Warning, TEXT("%s: Using fallback player controller"), *GetNameSafe(this));
        }
    }

    // Initialize settings
    ConfigureActivationSettings();
    
    // Set initial HUD scale
    UpdateHUDScale(DefaultHUDScale);

    // Validate initial state
    ValidateHUDState();
}

void UDefaultHUDLayout::NativeDestruct()
{
    OwningPC = nullptr;
    Super::NativeDestruct();
}

void UDefaultHUDLayout::ConfigureActivationSettings()
{
    SetHUDInputBlocking(false);
    MakeHUDSelfHitTestInvisible();
}

void UDefaultHUDLayout::SetHUDInputBlocking(bool bBlockInput)
{
    SetInputActionBlocking(bBlockInput);
}

void UDefaultHUDLayout::MakeHUDSelfHitTestInvisible()
{
    SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UDefaultHUDLayout::NativeOnActivated()
{
    Super::NativeOnActivated();
    
    if (OwningPC)
    {
        OwningPC->SetShowMouseCursor(false);
    }
    else
    {
        UE_LOG(LogTemp, Warning, 
            TEXT("%s: Cannot set mouse cursor state - OwningPC is null. Use SetShowMouseCursor via Blueprint if needed."), 
            *GetNameSafe(this));
    }
}

void UDefaultHUDLayout::NativeOnDeactivated()
{
    Super::NativeOnDeactivated();
}

float UDefaultHUDLayout::UpdateHUDScale(float NewScale)
{
    // Validate scale constraints before applying
    ensureMsgf(ValidateHUDScaleRange(), 
        TEXT("%s: Invalid HUD scale range. Scale may not behave as expected."), *GetNameSafe(this));

    // Clamp scale between configured min/max values
    const float ClampedScale = FMath::Clamp(NewScale, MinHUDScale, MaxHUDScale);
    
    if (!FMath::IsNearlyEqual(NewScale, ClampedScale))
    {
        UE_LOG(LogTemp, Verbose, TEXT("%s: HUD scale %f was clamped to %f"), 
            *GetNameSafe(this), NewScale, ClampedScale);
    }

    CurrentHUDScale = ClampedScale;
    SetRenderScale(FVector2D(ClampedScale));
    
    return CurrentHUDScale;
}

bool UDefaultHUDLayout::ValidateHUDScaleRange()
{
    // Validate min/max scale relationship
    if (MinHUDScale > MaxHUDScale)
    {
        UE_LOG(LogTemp, Error, 
            TEXT("%s: MinHUDScale (%f) cannot be greater than MaxHUDScale (%f)"), 
            *GetNameSafe(this), MinHUDScale, MaxHUDScale);
        return false;
    }

    // Validate default scale is within range
    if (DefaultHUDScale < MinHUDScale || DefaultHUDScale > MaxHUDScale)
    {
        UE_LOG(LogTemp, Error, 
            TEXT("%s: DefaultHUDScale (%f) must be between MinHUDScale (%f) and MaxHUDScale (%f)"), 
            *GetNameSafe(this), DefaultHUDScale, MinHUDScale, MaxHUDScale);
        return false;
    }

    return true;
}

void UDefaultHUDLayout::ValidateHUDState()
{
    // Check activation focus settings
    if (bSupportsActivationFocus)
    {
        UE_LOG(LogTemp, Warning, 
            TEXT("%s: bSupportsActivationFocus is enabled. This may interfere with player controls. Disable if input issues occur."),
            *GetNameSafe(this));
    }

    // Validate scale configuration
    if (!ValidateHUDScaleRange())
    {
        UE_LOG(LogTemp, Error, 
            TEXT("%s: HUD scale configuration is invalid. Please check MinHUDScale, MaxHUDScale, and DefaultHUDScale in Blueprint."),
            *GetNameSafe(this));
    }

    // Validate proper ownership
    if (!OwningPC)
    {
        UE_LOG(LogTemp, Warning, 
            TEXT("%s: No valid PlayerController found. Some HUD features may be limited."),
            *GetNameSafe(this));
    }
}