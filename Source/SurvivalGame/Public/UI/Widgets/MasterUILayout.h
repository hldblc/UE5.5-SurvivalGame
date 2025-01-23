#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
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

    /** Push game inventory layout to the stack */
    UFUNCTION(BlueprintCallable, Category = "UI|Layout")
    UGameInventoryLayout* PushGameInventoryLayout();

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

    /** Active layout references */
    UPROPERTY(BlueprintReadOnly, Category = "UI|Runtime")
    TObjectPtr<UGameInventoryLayout> GameInventoryLayout;
};