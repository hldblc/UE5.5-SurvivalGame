// GameBaseCharacter.cpp
#include "Characters/GameBaseCharacter.h"
#include "Interfaces/ControllerInterface.h"
#include "Interfaces/PlayerInterface.h"

// Constructor implementation
AGameBaseCharacter::AGameBaseCharacter()
{
	// Set this character to call Tick() every frame
	PrimaryActorTick.bCanEverTick = true;
}

// BeginPlay implementation
void AGameBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	// Additional initialization
}

// Tick implementation
void AGameBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Tick logic
}

// SetupPlayerInputComponent implementation
void AGameBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// Input setup
}

// PlayerInterface implementation
void AGameBaseCharacter::UpdateItem_Implementation(E_ContainerType ContainerType, int32 Index, FItemStructure ItemInfo)
{
	// Get the controller - use a different variable name to avoid conflict
	AController* PlayerController = GetController();
    
	// Check if the controller implements ControllerInterface
	if (PlayerController && PlayerController->Implements<UControllerInterface>())
	{
		// Call the UpdateItemSlot method on the controller through the interface
		IControllerInterface::Execute_UpdateItemSlot(PlayerController, ContainerType, ItemInfo, Index);
	}
}