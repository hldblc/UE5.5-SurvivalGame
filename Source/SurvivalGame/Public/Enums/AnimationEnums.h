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
enum class E_EquipableAnimationStates : uint8
{
	DefaultState     UMETA(DisplayName = "DefaultState"),
	HatchetState     UMETA(DisplayName = "HatchetState"),
	PickaxeState     UMETA(DisplayName = "PickaxeState"),
	HammerState      UMETA(DisplayName = "HammerState"),
	MaceState        UMETA(DisplayName = "MaceState"),
	KnifeState		 UMETA(DisplayName = "KnifeState"),
	FishingRodState  UMETA(DisplayName = "FishingRodState"),
	HoeState         UMETA(DisplayName = "HoeState"),
	ShovelState      UMETA(DisplayName = "ShovelState"),
	AxeState         UMETA(DisplayName = "AxeState"),
	SwordState       UMETA(DisplayName = "SwordState"),
	SpearState       UMETA(DisplayName = "SpearState"),
	BowState         UMETA(DisplayName = "BowState"),
	StaffState       UMETA(DisplayName = "StaffState"),
	ShieldState      UMETA(DisplayName = "ShieldState"),
	SwimmingState    UMETA(DisplayName = "SwimmingState"),
	FishingState     UMETA(DisplayName = "FishingState"),
	Invalid     UMETA(HIDDEN)
};