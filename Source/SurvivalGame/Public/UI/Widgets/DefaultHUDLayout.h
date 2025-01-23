// DefaultHUDLayout.h
#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "GameFramework/PlayerController.h"
#include "DefaultHUDLayout.generated.h"

/**
 * @brief Main HUD layout widget that contains core gameplay UI elements
 * 
 * This widget serves as a container for persistent HUD elements that should always be visible
 * during gameplay. It is designed to not consume input so players can still control their character
 * while the HUD is displayed.
 */
UCLASS(Abstract, BlueprintType, Blueprintable, meta=(DisplayName = "Default HUD Layout"))
class SURVIVALGAME_API UDefaultHUDLayout : public UCommonActivatableWidget
{
    GENERATED_BODY()

public:
    /** Constructor that initializes core widget settings */
    explicit UDefaultHUDLayout(const FObjectInitializer& ObjectInitializer);

    //~ Begin UUserWidget Interface
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    //~ End UUserWidget Interface

    /**
     * Updates the scale of all HUD elements
     * @param NewScale - The desired scale factor (clamped between MinHUDScale and MaxHUDScale)
     * @return The actual scale applied after clamping
     */
    UFUNCTION(BlueprintCallable, Category = "HUD|Layout", meta=(ClampMin = "0.5", ClampMax = "2.0"))
    float UpdateHUDScale(float NewScale);

    /** Gets the current HUD scale */
    UFUNCTION(BlueprintPure, Category = "HUD|Layout")
    float GetCurrentHUDScale() const { return CurrentHUDScale; }

protected:
    /** Minimum allowed HUD scale for visibility */
    UPROPERTY(EditDefaultsOnly, Category = "HUD|Config", meta=(ClampMin = "0.1", ClampMax = "1.0"))
    float MinHUDScale = 0.5f;

    /** Maximum allowed HUD scale to prevent excessive size */
    UPROPERTY(EditDefaultsOnly, Category = "HUD|Config", meta=(ClampMin = "1.0", ClampMax = "5.0"))
    float MaxHUDScale = 2.0f;

    /** Default scale when HUD is first created */
    UPROPERTY(EditDefaultsOnly, Category = "HUD|Config")
    float DefaultHUDScale = 1.0f;

    //~ Begin UCommonActivatableWidget Interface
    virtual void NativeOnActivated() override;
    virtual void NativeOnDeactivated() override;
    //~ End UCommonActivatableWidget Interface

    /**
     * Configures input handling for the HUD
     * @param bBlockInput - Whether the HUD should block input (generally should be false)
     */
    UFUNCTION(BlueprintCallable, Category = "HUD|Input")
    void SetHUDInputBlocking(bool bBlockInput);

    /** Makes the HUD invisible to hit testing while still being visible */
    UFUNCTION(BlueprintCallable, Category = "HUD|Visibility")
    void MakeHUDSelfHitTestInvisible();

    /** Validates HUD configuration and logs issues */
    void ValidateHUDState();

private:
    /** Cached owner reference for quick access */
    UPROPERTY()
    TObjectPtr<APlayerController> OwningPC;

    /** Current scale of the HUD */
    UPROPERTY()
    float CurrentHUDScale;

    /** Sets up initial widget activation configuration */
    void ConfigureActivationSettings();

    /** Validates and enforces HUD scale constraints */
    bool ValidateHUDScaleRange();
};