#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "DefaultHUDLayout.h"
#include "GameInventoryLayout.h"
#include "MasterUILayout.generated.h"

class UCommonActivatableWidgetContainerBase;

/**
* @brief Master UI Layout that manages different UI stacks for game HUD, inventory, and menus
*/
UCLASS(Abstract, BlueprintType, Blueprintable)
class SURVIVALGAME_API UMasterUILayout : public UCommonUserWidget
{
    GENERATED_BODY()

public:
    UMasterUILayout(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable, Category = "UI|Layout")
    UDefaultHUDLayout* PushDefaultHUDLayout();

protected:
    /** Stack containers */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UCommonActivatableWidgetContainerBase> GameHUDStack;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UCommonActivatableWidgetContainerBase> GameInventoryStack;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UCommonActivatableWidgetContainerBase> GameMenuStack;

    /** Widget class references */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|Config")
    TSubclassOf<UGameInventoryLayout> GameInventoryLayoutClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|Config")
    TSubclassOf<UDefaultHUDLayout> DefaultHUDLayoutClass;

    /** Active layout references */
    UPROPERTY(BlueprintReadOnly, Category = "UI|Runtime")
    TObjectPtr<UDefaultHUDLayout> DefaultHUDLayout;
};