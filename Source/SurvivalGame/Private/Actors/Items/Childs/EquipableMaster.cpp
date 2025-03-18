// EquipableMaster.cpp


#include "Items/Childs/EquipableMaster.h"

#include "ItemEnums.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AEquipableMaster::AEquipableMaster()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	bAlwaysRelevant = true;

	EquipableSocketName = NAME_None;
	AnimationState = E_EquipableAnimationStates::DefaultState;
}

// Called when the game starts or when spawned
void AEquipableMaster::BeginPlay()
{
	Super::BeginPlay();
	
}




void AEquipableMaster::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// Add replicated properties here using DOREPLIFETIME macro
	// Example (when you add properties later):
	// DOREPLIFETIME(AEquipableMaster, YourReplicatedVariable);
}

void AEquipableMaster::GetEquipableInfo_Implementation(FName& OutSocketName,
	E_EquipableAnimationStates& OutEquipableAnimationState)
{
	EquipableSocketName = OutSocketName;
	AnimationState = OutEquipableAnimationState;
}


