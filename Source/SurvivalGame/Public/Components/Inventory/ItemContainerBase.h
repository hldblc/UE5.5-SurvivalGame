// ItemContainerBase.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/Struct/ItemStructure.h"
#include "Enums/ContainerType.h"
#include "ItemContainerBase.generated.h"


/**
 * @brief Base component class for handling item storage and management
 */
UCLASS(ClassGroup=(Inventory), Blueprintable, meta=(BlueprintSpawnableComponent))
class SURVIVALGAME_API UItemContainerBase : public UActorComponent
{
    GENERATED_BODY()

public:
    UItemContainerBase();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void BeginPlay() override;

protected:
    /** Container configuration */
    UPROPERTY(EditDefaultsOnly, Category = "Container|Config")
    E_ContainerType ContainerType;

    UPROPERTY(EditDefaultsOnly, Category = "Container|Config")
    int32 MaxSlots;

    UFUNCTION(BlueprintCallable, Category = "Container|Debug")
    void DebugContainerState();

    /** Container state */
    UPROPERTY(ReplicatedUsing = OnRep_Items)
    TArray<FItemStructure> Items;

    /** Cached owner reference */
    UPROPERTY()
    AActor* OwningActor;

    /** Initialize container slots */
    void InitializeContainer();

    /** Network replication */
    UFUNCTION()
    void OnRep_Items() {}

    UFUNCTION(Server, Reliable)
    void Server_AddItem(const FItemStructure& Item);

    UFUNCTION(BlueprintCallable, Category = "Container|Events")
    void UpdateUI(int32 Index, const FItemStructure& ItemInfo);

public:
    /** Find empty slot in container */
    UFUNCTION(BlueprintPure, Category = "Container|Operations")
    bool FindEmptySlot(bool& Success, int32& EmptyIndex) const;

    /** Add item to container */
    UFUNCTION(BlueprintCallable, Category = "Container|Operations")
    bool AddItem(const FItemStructure& Item);

};