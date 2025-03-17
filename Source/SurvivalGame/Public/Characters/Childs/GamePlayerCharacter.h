// GamePlayerCharacter.h

#pragma once

#include "CoreMinimal.h"
#include "Characters/GameBaseCharacter.h"
#include "Components/Inventory/Child/PlayerInventory.h"
#include "Components/Inventory/Child/PlayerHotbarComponent.h" 
#include "GamePlayerCharacter.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SURVIVALGAME_API AGamePlayerCharacter : public AGameBaseCharacter 
{
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    AGamePlayerCharacter();

    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config|Inventory")
    class UPlayerInventory* PlayerInventory;

    // Inside the AGamePlayerCharacter class declaration
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config|Inventory")
    class UPlayerHotbarComponent* PlayerHotbar;
    
protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    // Override PlayerInterface function
    virtual void OnSlotDrop_Implementation(int32 DroppedIndex,
                                         int32 FromIndex,
                                         E_ContainerType TargetContainer,
                                         E_ContainerType FromContainerType,
                                         E_ArmorType ArmorType) override;

    // Server RPC for slot drop operations
    UFUNCTION(Server, Reliable, WithValidation)
    void Server_SlotDrop(int32 DroppedIndex, 
                        int32 FromIndex, 
                        E_ContainerType TargetContainer, 
                        E_ContainerType FromContainer,
                        E_ArmorType ArmorType);
    
    bool Server_SlotDrop_Validate(int32 DroppedIndex, 
                                int32 FromIndex, 
                                E_ContainerType TargetContainer, 
                                E_ContainerType FromContainer,
                                E_ArmorType ArmorType);
    
    void Server_SlotDrop_Implementation(int32 DroppedIndex, 
                                      int32 FromIndex, 
                                      E_ContainerType TargetContainer, 
                                      E_ContainerType FromContainer,
                                      E_ArmorType ArmorType);

    
    UFUNCTION(BlueprintCallable)
    void ProcessSlotDrop(int32 DroppedIndex,
                         int32 FromIndex,
                         E_ContainerType TargetContainer, 
                         E_ContainerType FromContainer,
                         E_ArmorType ArmorType);


    virtual void ResetItem_Implementation(E_ContainerType Container, int32 Index) override;


    
};