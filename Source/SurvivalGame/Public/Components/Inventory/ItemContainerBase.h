// ItemContainerBase.h

// ReSharper disable All
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


    /** Find empty slot in container */
    UFUNCTION(BlueprintPure, Category = "Container|Operations")
    bool FindEmptySlot(bool& Success, int32& EmptyIndex) const;

    /** Add item to container */
    UFUNCTION(BlueprintCallable, Category = "Container|Operations")
    bool AddItem(const FItemStructure& Item);

    UFUNCTION(Server, Reliable)
    void OnSlotDrop(UItemContainerBase* FromContainer, int32 FromItemIndex, int32 DroppedItemIndex);

    UFUNCTION(BlueprintCallable, Category = "Container|Operations")
    virtual void HandleSlotDrop (UItemContainerBase* HandleFromContainer, int32 HandleFromItemIndex, int32 HandleDroppedItemIndex);

    UFUNCTION(BlueprintCallable, Category = "Container|Operations")
    void TransferItem(UItemContainerBase* ToComponent, int32 ToSpecificIndex, int32 ItemIndexToTransfer);

    UFUNCTION(BlueprintPure, Category = "Container|Operations")
    bool IsSlotEmpty(int32 SlotIndex) const;

    UFUNCTION(BlueprintPure, Category = "Container|Operations")
    FItemStructure GetItemAtIndex(int32 Index) const;

    UFUNCTION(BlueprintCallable, Category = "Container|Operations")
    virtual void AddItemToIndex(const FItemStructure& ItemInfo, int32 LocalSpecificIndex, int32 LocalItemIndex, bool& Success);

    UFUNCTION(BlueprintCallable, Category = "Container|Debug")
    void PrintInventoryContents();

    UFUNCTION(BlueprintCallable, Category = "Container|Debug")
    virtual void RemoveItemAtIndex(int32 RemovedIndex, bool& Success);

    

};
















