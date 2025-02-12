// ItemContainerBase.cpp

#include "Components/Inventory/ItemContainerBase.h"

#include "Interfaces/PlayerInterface.h"
#include "Net/UnrealNetwork.h"

UItemContainerBase::UItemContainerBase()
{
    // Disable tick by default for performance
    PrimaryComponentTick.bCanEverTick = false;
    
    // Enable replication
    SetIsReplicatedByDefault(true);

    // Default configuration
    MaxSlots = 20;
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

//===========================================Server_AddItem====================================================
void UItemContainerBase::Server_AddItem_Implementation(const FItemStructure& Item)
{
    AddItem(Item);
}

//===========================================UpdateUI====================================================


void UItemContainerBase::UpdateUI(int32 Index, const FItemStructure& ItemInfo)
{
    AActor* OwnerActor = GetOwner();
    if (!OwnerActor)
    {
        return;
    }

    switch (ContainerType)
    {
    case E_ContainerType::Inventory:
        break;
    default:
        break;
    }
    
    if (OwnerActor->Implements<UPlayerInterface>())
    {
        IPlayerInterface::Execute_UpdateItem(OwnerActor,ContainerType, ItemInfo, Index);
    } 
    

}

//===========================================FindEmptySlot====================================================
bool UItemContainerBase::FindEmptySlot(bool& Success, int32& EmptyIndex) const
{
    // Initialize with invalid state
    Success = false;
    EmptyIndex = -1;

    for (int32 Index = 0; Index <Items.Num(); ++Index)
    {
        if (Items[Index].RegistryKey.IsNone())
        {
            Success = true;
            EmptyIndex = Index;
            break;
        }
    }

    return Success;

}
//===========================================AddItem====================================================
bool UItemContainerBase::AddItem(const FItemStructure& Item)
{
    if (GetOwnerRole() != ROLE_Authority)
    {
        Server_AddItem(Item);
        return true;
    }
    
    //Local copy of Item Info
    FItemStructure LocalItemInfo = Item;

    //Find an empty slot
    bool Success;
    int32 LocalEmptyIndex;
    FindEmptySlot(Success, LocalEmptyIndex);

    if (Success)
    {
        Items[LocalEmptyIndex] = LocalItemInfo;

        UpdateUI(LocalEmptyIndex, LocalItemInfo);
        return true;
    }

    return false;
}
