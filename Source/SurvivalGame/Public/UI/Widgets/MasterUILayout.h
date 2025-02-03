#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"  // Base class from CommonUI.
#include "DefaultHUDLayout.h"
#include "GameInventoryLayout.h"
#include "MasterUILayout.generated.h"

class UCommonActivatableWidgetContainerBase;

/**
 * @brief Master UI Layout managing the different CommonUI stacks (HUD, inventory, menus).
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class SURVIVALGAME_API UMasterUILayout : public UCommonUserWidget
{
    GENERATED_BODY()

public:
    // Constructor.
    UMasterUILayout(const FObjectInitializer& ObjectInitializer);

    /**
     * Push the default HUD layout onto the HUD stack.
     * @return The pushed UDefaultHUDLayout pointer.
     */
    UFUNCTION(BlueprintCallable, Category = "UI|Layout")
    UDefaultHUDLayout* PushDefaultHUDLayout();

    /**
     * Push the game inventory layout onto the inventory stack.
     * @return The pushed UGameInventoryLayout pointer.
     */
    UFUNCTION(BlueprintCallable, Category = "UI|Layout")
    UGameInventoryLayout* PushGameInventoryLayout();

    /**
     * Pop (remove) the game inventory layout from the inventory stack.
     */
    UFUNCTION(BlueprintCallable, Category = "UI")
    void PopGameInventoryLayout();

    /** Currently active game inventory layout widget (bound via UMG). */
    UPROPERTY(BlueprintReadWrite, Category = "UI")
    TObjectPtr<UGameInventoryLayout> GameInventoryLayout;


protected:
    /** Container for the HUD widgets (set up in UMG using CommonUI containers). */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UCommonActivatableWidgetContainerBase> GameHUDStack;

    /** Container for the inventory widgets. */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UCommonActivatableWidgetContainerBase> GameInventoryStack;

    /** Container for the menu widgets. */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UCommonActivatableWidgetContainerBase> GameMenuStack;

    /** Widget class for the game inventory layout (set in the Editor). */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|Config")
    TSubclassOf<UGameInventoryLayout> GameInventoryLayoutClass;

    /** Widget class for the default HUD layout (set in the Editor). */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|Config")
    TSubclassOf<UDefaultHUDLayout> DefaultHUDLayoutClass;

    /** Currently active default HUD layout widget. */
    UPROPERTY(BlueprintReadOnly, Category = "UI|Runtime")
    TObjectPtr<UDefaultHUDLayout> DefaultHUDLayout;


};
