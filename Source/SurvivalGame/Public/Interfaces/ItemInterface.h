// ItemInterface.h

#pragma once

#include "CoreMinimal.h"
#include "ItemEnums.h"
#include "UObject/Interface.h"
#include "ItemInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UItemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SURVIVALGAME_API IItemInterface
{
	GENERATED_BODY()

	
public:

	/**
	 * Retrieves detailed drag-and-drop information for an item from a specific slot.
	 *
	 * @param InSlotIndex The index of the slot from which the item information is being requested.
	 * @param InFromContainer The container type where the item currently resides.
	 * @param OutItemCategory The category of the item being retrieved.
	 * @param OutItemType The specific type of the item being retrieved.
	 * @param OutArmorType The armor type of the item, if applicable.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item")
	void GetItemDragDropInfo(int32 InSlotIndex,
							 E_ContainerType InFromContainer,
							 E_ItemCategory OutItemCategory,
							 E_ItemType OutItemType,
							 E_ArmorType OutArmorType);
};
