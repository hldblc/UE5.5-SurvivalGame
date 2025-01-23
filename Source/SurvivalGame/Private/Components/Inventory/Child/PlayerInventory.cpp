// PlayerInventory.cpp

#include "Components/Inventory/Child/PlayerInventory.h"
#include "Net/UnrealNetwork.h"

UPlayerInventory::UPlayerInventory()
{
    // Initialize default values
    DefaultInventorySize = 20;
    MaxInventorySize = 50;
    BaseWeightLimit = 100.0f;
    
    bAutoStackItems = true;
    MinLevelForExpansion = 5;
    SlotsPerExpansion = 5;
    
    CurrentWeight = 0.0f;
    MaxWeight = BaseWeightLimit;
    
    // Set container type
    ContainerType = E_ContainerType::Inventory;
}

void UPlayerInventory::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME(UPlayerInventory, CurrentWeight);
    DOREPLIFETIME(UPlayerInventory, MaxWeight);
}

void UPlayerInventory::BeginPlay()
{
    Super::BeginPlay();
    InitializeInventory();
}

void UPlayerInventory::InitializeInventory()
{
    if (GetOwnerRole() == ROLE_Authority)
    {
        // Set initial inventory size
        MaxSlots = DefaultInventorySize;
        MaxWeight = BaseWeightLimit;
        
        // Initialize weight
        UpdateCurrentWeight();
    }
}

bool UPlayerInventory::ExpandInventory(int32 AdditionalSlots)
{
    if (GetOwnerRole() != ROLE_Authority)
    {
        return false;
    }

    // Validate expansion
    if (!CheckLevelRequirement(FItemStructure()) || 
        MaxSlots + AdditionalSlots > MaxInventorySize)
    {
        return false;
    }

    HandleInventoryExpansion(MaxSlots + AdditionalSlots);
    return true;
}

void UPlayerInventory::HandleInventoryExpansion(int32 NewSize)
{
    // Store old size for the event
    int32 OldSize = MaxSlots;
    
    // Expand the inventory
    MaxSlots = FMath::Min(NewSize, MaxInventorySize);

    // Add new empty slots
    if (MaxSlots > OldSize)
    {
        Items.SetNum(MaxSlots);
        NotifyContainerUpdated();
        OnInventoryExpanded.Broadcast(OldSize, MaxSlots);
    }
}

bool UPlayerInventory::ValidateItem(const FItemStructure& Item) const
{
    if (!Super::ValidateItem(Item))
    {
        return false;
    }

    return IsItemTypeAllowed(Item) && 
           CheckWeightLimit(Item) && 
           CheckLevelRequirement(Item);
}

bool UPlayerInventory::CheckWeightLimit(const FItemStructure& Item) const
{
    float newWeight = CurrentWeight + CalculateItemWeight(Item);
    return newWeight <= MaxWeight;
}

bool UPlayerInventory::CheckLevelRequirement(const FItemStructure& Item) const
{
    // TODO: Implement level check logic when player level system is ready
    return true;
}

bool UPlayerInventory::IsItemTypeAllowed(const FItemStructure& Item) const
{
    return !RestrictedItemTypes.Contains(Item.ItemType);
}

float UPlayerInventory::CalculateItemWeight(const FItemStructure& Item) const
{
    // Basic weight calculation based on weight class
    float baseWeight;
    switch (Item.WeightClass)
    {
        case E_WeightClass::Light:
            baseWeight = 1.0f;
            break;
        case E_WeightClass::Medium:
            baseWeight = 2.0f;
            break;
        case E_WeightClass::Heavy:
            baseWeight = 5.0f;
            break;
        default:
            baseWeight = 0.0f;
    }
    
    return baseWeight * Item.ItemQuantity;
}

void UPlayerInventory::UpdateCurrentWeight()
{
    if (GetOwnerRole() != ROLE_Authority)
    {
        return;
    }

    float oldWeight = CurrentWeight;
    float newWeight = 0.0f;

    for (const FItemStructure& Item : Items)
    {
        if (!Item.IsEmpty())
        {
            newWeight += CalculateItemWeight(Item);
        }
    }

    CurrentWeight = newWeight;

    // Broadcast weight change if it changed
    if (!FMath::IsNearlyEqual(oldWeight, CurrentWeight))
    {
        OnWeightChanged.Broadcast(CurrentWeight, MaxWeight);
    }
}

float UPlayerInventory::GetWeightPercentage() const
{
    return MaxWeight > 0.0f ? (CurrentWeight / MaxWeight) : 1.0f;
}

void UPlayerInventory::OnRep_CurrentWeight()
{
    OnWeightChanged.Broadcast(CurrentWeight, MaxWeight);
}

bool UPlayerInventory::TransferToNearbyContainer()
{
    // TODO: Implement nearby container detection and transfer logic
    return false;
}