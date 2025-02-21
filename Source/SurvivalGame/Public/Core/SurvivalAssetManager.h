// SurvivalAssetManager.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "SurvivalAssetManager.generated.h"

/**
 * Custom Asset Manager for Survival Game.
 */
UCLASS()
class SURVIVALGAME_API USurvivalAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	/** Called at startup to allow scanning/initialization logic. */
	virtual void StartInitialLoading() override;

private:
	/** Callback after async load finishes. */
	void OnItemsLoaded() const;
};
