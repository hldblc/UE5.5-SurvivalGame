#include "Components/Inventory/Child/PlayerHotbarComponent.h"

UPlayerHotbarComponent::UPlayerHotbarComponent()
{
	// Set default values
	ContainerType = E_ContainerType::Hotbar;
	MaxSlots = 8;
    
	// Set component properties
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UPlayerHotbarComponent::BeginPlay()
{
	Super::BeginPlay();
    
	// You could add hotbar-specific initialization here if needed
	UE_LOG(LogTemp, Log, TEXT("PlayerHotbarComponent: Initialized with %d slots"), MaxSlots);
}