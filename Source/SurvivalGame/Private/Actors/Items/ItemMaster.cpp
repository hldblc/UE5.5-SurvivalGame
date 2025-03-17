// ItemMaster.cpp
#include "Actors/Items/ItemMaster.h"
#include "Net/UnrealNetwork.h" // Required for replication

// Sets default values
AItemMaster::AItemMaster()
{
	// Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    
	// Enable replication for this actor
	bReplicates = true;
    
	// Optional: Set replication to always be relevant
	bAlwaysRelevant = true;
    
	// Optional: If this actor has a visual component, you'd also want to set:
	// SetReplicateMovement(true);
}

void AItemMaster::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
	// Add replicated properties here using DOREPLIFETIME macro
	// Example (when you add properties later):
	// DOREPLIFETIME(AItemMaster, YourReplicatedVariable);
}

// Called when the game starts or when spawned
void AItemMaster::BeginPlay()
{
	Super::BeginPlay();
}
