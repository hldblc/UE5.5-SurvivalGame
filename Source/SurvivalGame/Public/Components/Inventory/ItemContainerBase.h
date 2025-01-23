// ItemContainerBase.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/Struct/ItemStructure.h"
#include "Enums/ContainerType.h"
#include "ItemContainerBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnContainerUpdated, const TArray<FItemStructure>&, Items);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSlotUpdated, int32, SlotIndex, const FItemStructure&, Item);

/**
 * @brief Base component class for handling item storage and management
 * 
 * This component provides core functionality for storing, adding, removing, and managing items
 * in a container like inventory, storage, hotbar etc. Designed for network replication and
 * server authority.
 */
UCLASS(ClassGroup=(Inventory), Blueprintable, BlueprintType, meta=(
    BlueprintSpawnableComponent, 
    DisplayName="Item Container Component",
    Category="Inventory System",
    ShortTooltip="Base component for handling item storage and management"))
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

    UPROPERTY(EditDefaultsOnly, Category = "Container|Config")
    bool bAllowStacking;

    /** Container state */
    UPROPERTY(ReplicatedUsing = OnRep_Items, EditAnywhere, BlueprintReadWrite, Blueprintable, Category = "Item Structure")
    TArray<FItemStructure> Items;

    /** Cached owner reference */
    UPROPERTY()
    AActor* OwningActor;

    /** Network replication */
    UFUNCTION()
    void OnRep_Items();

public:
    /** Core container operations */
    UFUNCTION(BlueprintCallable, Category = "Container|Operations")
    bool AddItem(const FItemStructure& Item, int32 TargetSlot = -1);

    UFUNCTION(BlueprintPure, Category = "Container|Operations")
    bool CanAddItem(const FItemStructure& Item, int32 TargetSlot = -1) const;

    UFUNCTION(BlueprintCallable, Category = "Container|Operations")
    bool RemoveItem(int32 SlotIndex, int32 Amount = 1);

    UFUNCTION(BlueprintPure, Category = "Container|Operations")
    bool HasItem(const FName& ItemID, int32& OutQuantity) const;

    /** Container queries */
    UFUNCTION(BlueprintPure, Category = "Container|Queries")
    int32 GetFirstEmptySlot() const;

    UFUNCTION(BlueprintPure, Category = "Container|Queries")
    bool IsSlotEmpty(int32 SlotIndex) const;

    UFUNCTION(BlueprintPure, Category = "Container|Queries")
    const TArray<FItemStructure>& GetItems() const { return Items; }

    /** Events */
    UPROPERTY(BlueprintAssignable, Category = "Container|Events")
    FOnContainerUpdated OnContainerUpdated;

    UPROPERTY(BlueprintAssignable, Category = "Container|Events")
    FOnSlotUpdated OnSlotUpdated;

protected:
    /** Server-side validation */
    bool ValidateSlotIndex(int32 SlotIndex) const;
    virtual bool ValidateItem(const FItemStructure& Item) const;  // Added virtual keyword

    /** Helper functions */
    void InitializeContainer();
    void UpdateSlot(int32 SlotIndex, const FItemStructure& Item);
    void NotifyContainerUpdated();
    
    /** Server RPC */
    UFUNCTION(Server, Reliable)
    void Server_AddItem(const FItemStructure& Item, int32 TargetSlot);

    UFUNCTION(Server, Reliable)
    void Server_RemoveItem(int32 SlotIndex, int32 Amount);
};