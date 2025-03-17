// GamePlayerCharacter.cpp

#include "Characters/Childs/GamePlayerCharacter.h"
#include "Components/Inventory/ItemContainerBase.h"
#include "Components/Inventory/Child/PlayerInventory.h"
#include "Interfaces/ControllerInterface.h"
#include "Inventory/Child/PlayerHotbarComponent.h"
#include "UI/Widgets/Hotbar/PlayerHotbar.h"

// Sets default values
AGamePlayerCharacter::AGamePlayerCharacter()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    
    // Create and set up the inventory component
    PlayerInventory = CreateDefaultSubobject<UPlayerInventory>(TEXT("PlayerInventory"));
    PlayerInventory->SetIsReplicated(true);

    // Create and set up the hotbar component
    PlayerHotbar = CreateDefaultSubobject<UPlayerHotbarComponent>(TEXT("PlayerHotbar"));
    PlayerHotbar->SetIsReplicated(true);
}

// Called when the game starts or when spawned
void AGamePlayerCharacter::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void AGamePlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AGamePlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// ================================= OnSlotDrop Interface Function ================================= 
// Interface implementation
void AGamePlayerCharacter::OnSlotDrop_Implementation(int32 DroppedIndex, int32 FromIndex,
    E_ContainerType TargetContainer, E_ContainerType FromContainerType, E_ArmorType ArmorType)
{
    UE_LOG(LogTemp, Log, TEXT("OnSlotDrop_Implementation: DroppedIndex=%d, FromIndex=%d, TargetCont=%d, FromCont=%d"),
        DroppedIndex, FromIndex, (int)TargetContainer, (int)FromContainerType);
    
    // Call the server RPC
    Server_SlotDrop(DroppedIndex, FromIndex, TargetContainer, FromContainerType, ArmorType);
}

// Server RPC validation
bool AGamePlayerCharacter::Server_SlotDrop_Validate(int32 DroppedIndex, int32 FromIndex, 
                                                 E_ContainerType TargetContainer, 
                                                 E_ContainerType FromContainer,
                                                 E_ArmorType ArmorType)
{
    // Validate input parameters
    if (DroppedIndex < 0 || FromIndex < 0)
    {
        return false;
    }
    
    // Check if container types are valid
    if (TargetContainer == E_ContainerType::None || FromContainer == E_ContainerType::None)
    {
        return false;
    }
    
    return true;
}

// Server RPC implementation
void AGamePlayerCharacter::Server_SlotDrop_Implementation(int32 DroppedIndex, int32 FromIndex, 
                                                       E_ContainerType TargetContainer, 
                                                       E_ContainerType FromContainer,
                                                       E_ArmorType ArmorType)
{
    UE_LOG(LogTemp, Log, TEXT("Server_SlotDrop_Implementation: DroppedIndex=%d, FromIndex=%d, TargetCont=%d, FromCont=%d"),
        DroppedIndex, FromIndex, (int)TargetContainer, (int)FromContainer);
    
    // Call the processing function with the CORRECT parameter order
    ProcessSlotDrop(DroppedIndex, FromIndex, TargetContainer, FromContainer, ArmorType);
}

// Main processing function for slot drop operations
void AGamePlayerCharacter::ProcessSlotDrop(int32 DroppedIndex,
                                         int32 FromIndex,
                                         E_ContainerType TargetContainer,
                                         E_ContainerType FromContainer,
                                         E_ArmorType ArmorType)
{
    UE_LOG(LogTemp, Log, TEXT("ProcessSlotDrop: DroppedIndex=%d, FromIndex=%d, TargetCont=%d, FromCont=%d"),
        DroppedIndex, FromIndex, (int)TargetContainer, (int)FromContainer);
    
    // Get target container component - currently only supporting Inventory
    UPlayerInventory* TargetContainerComp = nullptr;
    if (TargetContainer == E_ContainerType::Inventory)
    {
        TargetContainerComp = PlayerInventory;
    }
    
    // Get source container component - currently only supporting Inventory
    UPlayerInventory* FromContainerComp = nullptr;
    if (FromContainer == E_ContainerType::Inventory)
    {
        FromContainerComp = PlayerInventory;
    }
    
    // Check if both container components are valid
    if (IsValid(TargetContainerComp) && IsValid(FromContainerComp))
    {
        // Cast to base container types
        UItemContainerBase* BaseTargetContainer = Cast<UItemContainerBase>(TargetContainerComp);
        UItemContainerBase* BaseFromContainer = Cast<UItemContainerBase>(FromContainerComp);
        
        if (BaseTargetContainer && BaseFromContainer)
        {
            UE_LOG(LogTemp, Log, TEXT("ProcessSlotDrop: About to call OnSlotDrop with BaseFromContainer=%p, FromIndex=%d, DroppedIndex=%d"),
                BaseFromContainer, FromIndex, DroppedIndex);
            
            BaseTargetContainer->OnSlotDrop(BaseFromContainer, FromIndex, DroppedIndex);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("ProcessSlotDrop: Failed to cast to base container class"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ProcessSlotDrop: Invalid container references. Target: %s, From: %s"),
            TargetContainerComp ? TEXT("Valid") : TEXT("Invalid"),
            FromContainerComp ? TEXT("Valid") : TEXT("Invalid"));
    }
}

void AGamePlayerCharacter::ResetItem_Implementation(E_ContainerType Container, int32 Index)
{
    if (Controller)
    {
        IControllerInterface::Execute_ResetItemSlot(Controller, Container, Index);
    }
}
