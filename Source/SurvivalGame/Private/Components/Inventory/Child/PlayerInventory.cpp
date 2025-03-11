// PlayerInventory.cpp

#include "Components/Inventory/Child/PlayerInventory.h"

#include "Interfaces/PlayerInterface.h"
#include "Net/UnrealNetwork.h"


UPlayerInventory::UPlayerInventory()
{
	PrimaryComponentTick.bCanEverTick = false;
	ContainerType = E_ContainerType::Inventory;
	MaxSlots = 60;
}

void UPlayerInventory::BeginPlay()
{
	Super::BeginPlay();
}

void UPlayerInventory::HandleSlotDrop(UItemContainerBase* HandleFromContainer, int32 HandleFromItemIndex,
	int32 HandleDroppedItemIndex)
{
	UE_LOG(LogTemp, Log, TEXT("PlayerInventory::HandleSlotDrop: FromContainer=%p, FromIndex=%d, DroppedIndex=%d"),
		HandleFromContainer, HandleFromItemIndex, HandleDroppedItemIndex);
        
	UItemContainerBase* LocalFromContainer = HandleFromContainer;
	int32 LocalFromItemIndex = HandleFromItemIndex;
	int32 LocalDroppedItemIndex = HandleDroppedItemIndex;

	if ((LocalFromContainer == this) && (LocalFromItemIndex == LocalDroppedItemIndex))
	{
		UE_LOG(LogTemp, Log, TEXT("HandleSlotDrop: Same slot in same container - ignoring"));
		return;
	}
    
	if (LocalFromContainer)
	{
		// Use a different variable name to avoid shadowing
		E_ContainerType FromContainerType = LocalFromContainer->ContainerType;

		switch (FromContainerType)
		{
		case E_ContainerType::Inventory:
		case E_ContainerType::Hotbar:
		case E_ContainerType::Storage:
		case E_ContainerType::AICompanion:
			UE_LOG(LogTemp, Log, TEXT("HandleSlotDrop: Transferring item from container type %d"),
				(int)FromContainerType);
                    
			// Transfer the item from source to destination
			LocalFromContainer->TransferItem(this, LocalDroppedItemIndex, LocalFromItemIndex);
			break;

		default:
			UE_LOG(LogTemp, Warning, TEXT("HandleSlotDrop: Unhandled container type %d"),
				(int)FromContainerType);
			break;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("HandleSlotDrop: From container is null"));
	}
}


void UPlayerInventory::AddItemToIndex(const FItemStructure& ItemInfo, int32 LocalSpecificIndex, int32 LocalItemIndex, bool& Success)
{
	UE_LOG(LogTemp, Log, TEXT("PlayerInventory::AddItemToIndex: ItemKey=%s, LocalIndex=%d, LocalFromIndex=%d"),
		*ItemInfo.RegistryKey.ToString(), LocalSpecificIndex, LocalItemIndex);
        
	Super::AddItemToIndex(ItemInfo, LocalSpecificIndex, LocalItemIndex, Success);

	if (Success && GetOwner() && GetOwner()->Implements<UPlayerInterface>())
	{
		// Make sure to use the correct parameter order and container type
		IPlayerInterface::Execute_UpdateItem(GetOwner(), this->ContainerType, LocalSpecificIndex, ItemInfo);
		UE_LOG(LogTemp, Log, TEXT("PlayerInventory::AddItemToIndex: Successfully updated UI"));
	}
	else if (!Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerInventory::AddItemToIndex: Operation was not successful"));
	}
}

void UPlayerInventory::RemoveItemAtIndex(int32 RemovedIndex, bool& Success)
{
	// Call parent implementation but avoid double-checking ownership/interface if operation fails
	Super::RemoveItemAtIndex(RemovedIndex, Success);

	// Early exit if operation failed - saves unnecessary checks
	if (!Success)
	{
#if !UE_BUILD_SHIPPING
		UE_LOG(LogTemp, Verbose, TEXT("PlayerInventory::RemoveItemAtIndex: Failed for index %d"), RemovedIndex);
#endif
		return;
	}
    
	// Cache owner to avoid multiple GetOwner() calls which traverse object hierarchy
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}
    
	// Use static_cast for performance if we're confident the interface is implemented
	// Alternatively, keep the safer check with a cached result
	static bool bCheckedInterface = false;
	static bool bOwnerImplementsInterface = false;
    
	if (!bCheckedInterface || Owner != CachedOwner)
	{
		bCheckedInterface = true;
		CachedOwner = Owner;
		bOwnerImplementsInterface = Owner->Implements<UPlayerInterface>();
	}
    
	if (bOwnerImplementsInterface)
	{
		// Cache container type to avoid member access in hot path
		const E_ContainerType CurrentContainer = ContainerType;
        
		// Execute interface method to reset UI
		IPlayerInterface::Execute_ResetItem(Owner, CurrentContainer, RemovedIndex);
	}
}
