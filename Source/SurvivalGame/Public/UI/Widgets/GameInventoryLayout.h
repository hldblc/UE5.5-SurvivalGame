#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "UI/Widgets/InventoryWidget.h"
#include "GameInventoryLayout.generated.h"

class UWidgetSwitcher;
class UInventoryWidget;



/**
 * @brief Layout widget that manages the game's inventory interface tabs
 * Blueprint: WBP_GameInventoryLayout
 */
UCLASS(BlueprintType, Blueprintable, meta = (DisplayName = "Game Inventory Layout"))
class SURVIVALGAME_API UGameInventoryLayout : public UCommonActivatableWidget
{
    GENERATED_BODY()

public:
    UGameInventoryLayout(const FObjectInitializer& ObjectInitializer);


};