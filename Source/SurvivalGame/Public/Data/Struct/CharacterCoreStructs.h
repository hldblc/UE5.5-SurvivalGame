// CharacterCoreStructs.h

#pragma once

#include "CoreMinimal.h"
#include "CharacterCoreStructs.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct SURVIVALGAME_API FCharacterInputState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Input")
	bool WantsToSprint = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Input")
	bool WantsToWalk = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Input")
	bool WantsToStrafe = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Input")
	bool WantsToAim = false;
};

