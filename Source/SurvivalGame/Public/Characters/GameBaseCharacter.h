// In GameBaseCharacter.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/PlayerInterface.h"  // Include the PlayerInterface
#include "Enums/ContainerType.h"
#include "Data/Struct/ItemStructure.h"
#include "GameBaseCharacter.generated.h"

UCLASS()
class SURVIVALGAME_API AGameBaseCharacter : public ACharacter, public IPlayerInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGameBaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// PlayerInterface implementation
	virtual void UpdateItem_Implementation(E_ContainerType ContainerType, int32 Index, FItemStructure ItemInfo) override;
};