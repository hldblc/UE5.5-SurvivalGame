// ItemContainerBase.cpp

#include "Components/Inventory/ItemContainerBase.h"
#include "Net/UnrealNetwork.h"

UItemContainerBase::UItemContainerBase()
{
    // Disable tick by default for performance
    PrimaryComponentTick.bCanEverTick = false;
    
    // Enable replication
    SetIsReplicatedByDefault(true);

    // Default configuration
    MaxSlots = 20;
    bAllowStacking = true;
    ContainerType = E_ContainerType::Storage;
}

void UItemContainerBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // Replicate items array to all clients
    DOREPLIFETIME(UItemContainerBase, Items);
}

void UItemContainerBase::BeginPlay()
{
    Super::BeginPlay();
    
    // Cache owner reference
    OwningActor = GetOwner();
    
    // Initialize container slots
    InitializeContainer();
}

void UItemContainerBase::InitializeContainer()
{
    if (GetOwnerRole() == ROLE_Authority)
    {
        // Initialize with empty slots
        Items.SetNum(MaxSlots);
        for (int32 i = 0; i < MaxSlots; ++i)
        {
            Items[i] = FItemStructure(); // Creates empty item structure
        }
    }
}

bool UItemContainerBase::CanAddItem(const FItemStructure& Item, int32 TargetSlot) const
{
    // If a specific slot is targeted, check if it's valid and can accept the item
    if (TargetSlot >= 0)
    {
        if (!ValidateSlotIndex(TargetSlot))
        {
            return false;
        }

        // If slot is empty, it can accept the item
        if (IsSlotEmpty(TargetSlot))
        {
            return true;
        }

        // If stacking is allowed and items are stackable, check if we can add to the stack
        if (bAllowStacking && Items[TargetSlot].RegistryKey == Item.RegistryKey)
        {
            int32 spaceInStack = Items[TargetSlot].MaxStackSize - Items[TargetSlot].ItemQuantity;
            return spaceInStack > 0;
        }

        return false;
    }

    // Check for any available slot that can accept the item
    for (int32 i = 0; i < Items.Num(); ++i)
    {
        // Check empty slots
        if (IsSlotEmpty(i))
        {
            return true;
        }

        // Check stackable slots
        if (bAllowStacking && 
            Items[i].RegistryKey == Item.RegistryKey &&
            Items[i].ItemQuantity < Items[i].MaxStackSize)
        {
            return true;
        }
    }

    return false;
}

bool UItemContainerBase::AddItem(const FItemStructure& Item, int32 TargetSlot)
{
    // Client-side request
    if (GetOwnerRole() != ROLE_Authority)
    {
        Server_AddItem(Item, TargetSlot);
        return true;
    }

    if (!ValidateItem(Item))
    {
        return false;
    }

    // Handle specific slot request
    if (TargetSlot >= 0)
    {
        if (!ValidateSlotIndex(TargetSlot))
        {
            return false;
        }

        if (IsSlotEmpty(TargetSlot))
        {
            UpdateSlot(TargetSlot, Item);
            return true;
        }
        else if (bAllowStacking && Items[TargetSlot].RegistryKey == Item.RegistryKey)
        {
            // Stack items if possible
            int32 spaceInStack = Items[TargetSlot].MaxStackSize - Items[TargetSlot].ItemQuantity;
            if (spaceInStack > 0)
            {
                int32 amountToAdd = FMath::Min(spaceInStack, Item.ItemQuantity);
                Items[TargetSlot].ItemQuantity += amountToAdd;
                UpdateSlot(TargetSlot, Items[TargetSlot]);
                return true;
            }
        }
        return false;
    }

    // Find suitable slot if no specific target
    int32 emptySlot = GetFirstEmptySlot();
    if (emptySlot != -1)
    {
        UpdateSlot(emptySlot, Item);
        return true;
    }

    return false;
}

bool UItemContainerBase::RemoveItem(int32 SlotIndex, int32 Amount)
{
    // Client-side request
    if (GetOwnerRole() != ROLE_Authority)
    {
        Server_RemoveItem(SlotIndex, Amount);
        return true;
    }

    if (!ValidateSlotIndex(SlotIndex) || IsSlotEmpty(SlotIndex))
    {
        return false;
    }

    FItemStructure& SlotItem = Items[SlotIndex];
    if (SlotItem.ItemQuantity <= Amount)
    {
        // Remove entire stack
        UpdateSlot(SlotIndex, FItemStructure());
    }
    else
    {
        // Remove partial stack
        SlotItem.ItemQuantity -= Amount;
        UpdateSlot(SlotIndex, SlotItem);
    }

    return true;
}

bool UItemContainerBase::HasItem(const FName& ItemID, int32& OutQuantity) const
{
    OutQuantity = 0;
    for (const FItemStructure& Item : Items)
    {
        if (Item.RegistryKey == ItemID)
        {
            OutQuantity += Item.ItemQuantity;
        }
    }
    return OutQuantity > 0;
}

int32 UItemContainerBase::GetFirstEmptySlot() const
{
    for (int32 i = 0; i < Items.Num(); ++i)
    {
        if (IsSlotEmpty(i))
        {
            return i;
        }
    }
    return -1;
}

bool UItemContainerBase::IsSlotEmpty(int32 SlotIndex) const
{
    return ValidateSlotIndex(SlotIndex) && Items[SlotIndex].IsEmpty();
}

void UItemContainerBase::UpdateSlot(int32 SlotIndex, const FItemStructure& Item)
{
    if (!ValidateSlotIndex(SlotIndex))
    {
        return;
    }

    Items[SlotIndex] = Item;
    OnSlotUpdated.Broadcast(SlotIndex, Item);
    NotifyContainerUpdated();
}

void UItemContainerBase::NotifyContainerUpdated()
{
    OnContainerUpdated.Broadcast(Items);
}

void UItemContainerBase::OnRep_Items()
{
    NotifyContainerUpdated();
}

bool UItemContainerBase::ValidateSlotIndex(int32 SlotIndex) const
{
    return SlotIndex >= 0 && SlotIndex < MaxSlots;
}

bool UItemContainerBase::ValidateItem(const FItemStructure& Item) const
{
    // Basic validation
    return !Item.IsEmpty() && Item.ItemQuantity > 0;
}

void UItemContainerBase::Server_AddItem_Implementation(const FItemStructure& Item, int32 TargetSlot)
{
    AddItem(Item, TargetSlot);
}

void UItemContainerBase::Server_RemoveItem_Implementation(int32 SlotIndex, int32 Amount)
{
    RemoveItem(SlotIndex, Amount);
}