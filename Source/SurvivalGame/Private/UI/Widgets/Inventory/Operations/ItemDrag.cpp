// ItemDrag.cpp 

#include "UI/Widgets/Inventory/Operations/ItemDrag.h"

void UItemDrag::GetItemDragDropInfo_Implementation(int32 InSlotIndex,
												  E_ContainerType InFromContainer,
												  E_ItemCategory OutItemCategory,
												  E_ItemType OutItemType,
												  E_ArmorType OutArmorType)
{
	// Set the output parameters to the values from our class members
	InSlotIndex = SlotIndex;
	InFromContainer = FromContainer;
	OutItemCategory = ItemCategory;
	OutItemType = ItemType;
	OutArmorType = ArmorType;
}