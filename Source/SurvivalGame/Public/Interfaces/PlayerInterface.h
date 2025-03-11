#pragma once

#include "UObject/Interface.h"
#include "Enums/ContainerType.h"
#include "Data/Struct/ItemStructure.h"
#include "PlayerInterface.generated.h"


UINTERFACE(BlueprintType)
class SURVIVALGAME_API UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

class SURVIVALGAME_API IPlayerInterface
{
	GENERATED_BODY()

public:
	/**
	 * Updates an item in a specified container at the given index.
	 * This method is used for modifying or replacing an item entry within a player's inventory or other containers.
	 *
	 * @param ContainerType The type of container where the item resides (e.g., Inventory, Hotbar, Storage).
	 * @param ItemInfo The information of the item to be updated, including its properties and stats.
	 * @param Index The zero-based index within the container where the item should be updated.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
	void UpdateItem(E_ContainerType ContainerType, int32 Index, FItemStructure ItemInfo);
	

	/**
 * Handles the event when a slot drop occurs, allowing interaction with item management systems.
 *
 * @param DroppedIndex The index of the slot where the item is being dropped into.
 * @param FromIndex The index of the slot from which the item is being moved.
 * @param TargetContainer The type of container where the item is being dropped.
 * @param FromContainerType The type of container the item is being moved from.
 * @param ArmorType The type of armor related to the operation, if applicable.
 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item")
	void OnSlotDrop(int32 DroppedIndex,
					int32 FromIndex,
					E_ContainerType TargetContainer,
					E_ContainerType FromContainerType,
					E_ArmorType ArmorType);


	/**
	 * Resets an item in a specified container at the given index.
	 * This method is used to clear or restore an item's state within a player's inventory
	 * or other containers.
	 *
	 * @param Container The type of container where the item resides (e.g., Inventory, Hotbar, Storage).
	 * @param Index The zero-based index within the container where the item should be reset.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item")
	void ResetItem(E_ContainerType Container, int32 Index);
};
