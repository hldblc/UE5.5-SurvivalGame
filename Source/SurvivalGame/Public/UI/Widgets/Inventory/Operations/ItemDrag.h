#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "Enums/ContainerType.h"
#include "Enums/ItemEnums.h"
#include "Interfaces/ItemInterface.h"
#include "ItemDrag.generated.h"

/**
 * Custom drag drop operation class for inventory system.
 * Stores information about the dragged inventory item.
 */
UCLASS(Blueprintable, BlueprintType)

class SURVIVALGAME_API UItemDrag : public UDragDropOperation, public IItemInterface
{
	GENERATED_BODY()

public:
	/** The slot index of the source */
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	int32 SlotIndex;
    
	/** The container type of the source slot */
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	E_ContainerType FromContainer;
    
	/** The category of the item being dragged */
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	E_ItemCategory ItemCategory;

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	E_ItemType ItemType;
    
	/** The armor type (if applicable) */
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	E_ArmorType ArmorType;


	// =================ItemInterface Implementation=================
	virtual void GetItemDragDropInfo_Implementation(int32 SlotIndex,
													E_ContainerType InFromContainer,
													E_ItemCategory OutItemCategory,
													E_ItemType OutItemType,
													E_ArmorType OutArmorType) override;
};