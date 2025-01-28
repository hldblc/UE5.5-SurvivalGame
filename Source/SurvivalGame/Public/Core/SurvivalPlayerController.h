// SurvivalPlayerController.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UI/Widgets/DefaultHUDLayout.h"
#include "InputMappingContext.h"
#include "UObject/SoftObjectPtr.h"
#include "SurvivalPlayerController.generated.h"

class UInputAction;
class UMasterUILayout;

/**
 * @brief Player controller for the survival game
 */
UCLASS(Blueprintable, BlueprintType)
class SURVIVALGAME_API ASurvivalPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ASurvivalPlayerController();

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    /** Input Properties */
    UPROPERTY(EditDefaultsOnly, Category = "Input|Actions")
    TObjectPtr<UInputAction> IA_InventoryToggle;
    
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputMappingContext> DefaultInputMapping;

    /** UI Properties */
    UPROPERTY(EditDefaultsOnly, Category="UI|Class Properties")
    TSubclassOf<UMasterUILayout> MasterLayoutClass;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="UI|References")
    TObjectPtr<UMasterUILayout> RootLayout;

    /** State */
    UPROPERTY(Transient)
    bool bInventoryShown{false};

    /** UI Functions */
    UFUNCTION(Client, Reliable)
    void InventoryOnClient();

private:
    /** Initialization */
    void InitializeEnhancedInput();
    void CreateMasterLayout();
};