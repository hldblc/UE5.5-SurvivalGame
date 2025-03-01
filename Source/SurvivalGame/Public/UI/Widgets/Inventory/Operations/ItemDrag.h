#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "Enums/ContainerType.h"
#include "Enums/ItemEnums.h"
#include "ItemDrag.generated.h"

/**
 * Custom drag drop operation class for inventory system.
 * Stores information about the dragged inventory item.
 */
UCLASS()
class SURVIVALGAME_API UItemDrag : public UDragDropOperation
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
    
	/** The armor type (if applicable) */
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	E_ArmorType ArmorType;
};