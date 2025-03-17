#pragma once

#include "CoreMinimal.h"
#include "Components/Inventory/ItemContainerBase.h"
#include "PlayerHotbarComponent.generated.h"

/**
 * @brief Component for managing player's hotbar items.
 * 
 * This component extends the base item container to specifically handle
 * quick-access items in the player's hotbar.
 */
UCLASS(ClassGroup=(Inventory), Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent, 
	DisplayName="Player Hotbar Component",
	Category="Inventory System"))
class SURVIVALGAME_API UPlayerHotbarComponent : public UItemContainerBase
{
	GENERATED_BODY()

public:
	UPlayerHotbarComponent();

protected:
	/** Configure the default values in BeginPlay */
	virtual void BeginPlay() override;
};