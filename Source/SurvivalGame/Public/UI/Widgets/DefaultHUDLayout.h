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

};