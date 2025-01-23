#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SurvivalPlayerController.generated.h"

class UInputAction;
class UMasterUILayout;

/**
 * @brief Player controller for the survival game
 */
UCLASS()
class SURVIVALGAME_API ASurvivalPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ASurvivalPlayerController();

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

    /** Input handling */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> IA_InventoryToggle;

    /** UI References */
    UPROPERTY(BlueprintReadOnly, Category = "UI")
    TObjectPtr<UMasterUILayout> RootLayout;

    /** State */
    UPROPERTY(Replicated)
    bool bInventoryShown;

    /** Input handlers */
    void HandleInventoryToggle(const struct FInputActionInstance& Instance);

    /** Client-side inventory functions */
    UFUNCTION(Client, Reliable)
    void InventoryOnClient();

    /** Helper functions */
    void UpdateInputMode(bool bShowUI);
    void SetMouseCursorVisibility(bool bShow);
};