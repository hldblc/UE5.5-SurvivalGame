// PlayerInventory.h

#pragma once

#include "CoreMinimal.h"
#include "Components/Inventory/ItemContainerBase.h"
#include "Data/Struct/ItemStructure.h"
#include "Enums/ItemEnums.h"
#include "PlayerInventory.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeightChanged, float, NewWeight, float, MaxWeight);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryExpanded, int32, OldSize, int32, NewSize);

/**
 * @brief Player-specific inventory component that handles all player item management
 * 
 * Extends base container functionality with player-specific features like:
 * - Inventory expansion based on player level
 * - Weight management
 * - Quick transfers
 * - Level-based item restrictions
 */
UCLASS(ClassGroup=(Inventory), Blueprintable, BlueprintType, meta=(
    BlueprintSpawnableComponent, 
    DisplayName="Player Container Component",
    Category="Inventory System",
    ShortTooltip="Player component for handling item storage and management for players"))
class SURVIVALGAME_API UPlayerInventory : public UItemContainerBase
{
    GENERATED_BODY()


};