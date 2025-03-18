// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimationEnums.h"
#include "UObject/Interface.h"
#include "EquipableItemInterface.generated.h"

/**
 * 
 */
UINTERFACE(BlueprintType)
class SURVIVALGAME_API UEquipableItemInterface : public UInterface
{
	GENERATED_BODY()
};

class SURVIVALGAME_API IEquipableItemInterface
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipable")
	void GetEquipableInfo(FName& OutSocketName, E_EquipableAnimationStates& OutEquipableAnimationState);
	
};