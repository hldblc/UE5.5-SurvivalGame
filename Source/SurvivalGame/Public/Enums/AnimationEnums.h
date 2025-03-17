// AnimationEnums

#pragma once

#include "CoreMinimal.h"
#include "AnimationEnums.generated.h"


/**
 * @enum E_EquipableState
 * Defines the various equipable states an item can have in the game.
 * Each state corresponds to a specific type or status of an item, used for categorizing
 * or determining unique behaviors and animations in gameplay.
 */
UENUM(BlueprintType)
enum class E_EquipableStates : uint8
{
	None        UMETA(DisplayName = "Default"),
	Hatchet     UMETA(DisplayName = "Hatchet"),
	Sword       UMETA(DisplayName = "Sword"),
	Axe         UMETA(DisplayName = "Axe"),
	Spear       UMETA(DisplayName = "Spear"),
	Bow         UMETA(DisplayName = "Bow"),
	Staff       UMETA(DisplayName = "Staff"),
	Shield      UMETA(DisplayName = "Shield"),
	Swimming    UMETA(DisplayName = "Swimming"),
	Fishing     UMETA(DisplayName = "Fishing"),
	Invalid     UMETA(HIDDEN)
};