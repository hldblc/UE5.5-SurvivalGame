// EquipableMaster.cpp
#include "Items/Childs/EquipableMaster.h"
#include "ItemEnums.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AEquipableMaster::AEquipableMaster()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Enable replication inherited from ItemMaster
	bReplicates = true;
	bAlwaysRelevant = true;

	// Initialize default values
	EquipableSocketName = NAME_None;
	AnimationState = E_EquipableAnimationStates::DefaultState;
	bIsTwoHanded = false;
	TwoHandedSocketName = NAME_None;
}

// Called when the game starts or when spawned
void AEquipableMaster::BeginPlay()
{
	Super::BeginPlay();
}

void AEquipableMaster::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
	// Register our properties for replication
	DOREPLIFETIME(AEquipableMaster, EquipableSocketName);
	DOREPLIFETIME(AEquipableMaster, AnimationState);
	DOREPLIFETIME(AEquipableMaster, bIsTwoHanded);
	DOREPLIFETIME(AEquipableMaster, TwoHandedSocketName);
}

void AEquipableMaster::GetEquipableInfo_Implementation(FName& OutSocketName, 
													  E_EquipableAnimationStates& OutEquipableAnimationState,
													  bool& OutIsTwoHanded,
													  FName& OutTwoHandedSocketName)
{
	// This function should provide information to the caller, not set our own variables
	OutSocketName = EquipableSocketName;
	OutEquipableAnimationState = AnimationState;
	OutIsTwoHanded = bIsTwoHanded;
	OutTwoHandedSocketName = TwoHandedSocketName;
}