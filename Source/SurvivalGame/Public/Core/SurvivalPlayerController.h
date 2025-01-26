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
UCLASS(Blueprintable, blueprintType)
class SURVIVALGAME_API ASurvivalPlayerController : public APlayerController
{
    GENERATED_BODY()


   
protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

    bool bInventoryShown;

    UPROPERTY(EditDefaultsOnly, Category = "Input|Actions")
    TObjectPtr<UInputAction> IA_InventoryToggle;

    UFUNCTION(Client, Reliable)
    void InventoryOnClient();
    
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputMappingContext> DefaultInputMapping;

    UPROPERTY(EditDefaultsOnly, Category="UI|Class Properties")
    TSubclassOf<UMasterUILayout> MasterLayoutClass;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="UI|References")
    TObjectPtr<UMasterUILayout> RootLayout;

private:
    void InitializeEnhancedInput();
    void CreateMasterLayout();
    
};