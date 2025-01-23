// PlayerInventory.h

#pragma once

#include "CoreMinimal.h"
#include "Components/Inventory/ItemContainerBase.h"
#include "Data/Struct/ItemStructure.h"
#include "Enums/ItemEnums.h"
#include "PlayerInventory.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeightChanged, float, NewWeight, float, MaxWeight);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryExpanded, int32, OldSize, int32, NewSize);

/**
 * @brief Player-specific inventory component that handles all player item management
 * 
 * Extends base container functionality with player-specific features like:
 * - Inventory expansion based on player level
 * - Weight management
 * - Quick transfers
 * - Level-based item restrictions
 */
UCLASS(ClassGroup=(Inventory), Blueprintable, BlueprintType, meta=(
    BlueprintSpawnableComponent, 
    DisplayName="Player Container Component",
    Category="Inventory System",
    ShortTooltip="Player component for handling item storage and management for players"))
class SURVIVALGAME_API UPlayerInventory : public UItemContainerBase
{
    GENERATED_BODY()

public:
    UPlayerInventory();
    virtual void BeginPlay() override;

protected:
    /** Core inventory settings */
    UPROPERTY(EditDefaultsOnly, Category = "Inventory|Config")
    int32 DefaultInventorySize;

    UPROPERTY(EditDefaultsOnly, Category = "Inventory|Config")
    int32 MaxInventorySize;

    UPROPERTY(EditDefaultsOnly, Category = "Inventory|Config")
    float BaseWeightLimit;

    /** Item handling rules */
    UPROPERTY(EditDefaultsOnly, Category = "Inventory|Rules")
    bool bAutoStackItems;

    UPROPERTY(EditDefaultsOnly, Category = "Inventory|Rules")
    TArray<E_ItemType> RestrictedItemTypes;

    /** Level requirements */
    UPROPERTY(EditDefaultsOnly, Category = "Inventory|Progression")
    int32 MinLevelForExpansion;

    UPROPERTY(EditDefaultsOnly, Category = "Inventory|Progression")
    int32 SlotsPerExpansion;

    /** Weight system */
    UPROPERTY(ReplicatedUsing = OnRep_CurrentWeight)
    float CurrentWeight;

    UPROPERTY(Replicated)
    float MaxWeight;

public:
    /** Events */
    UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
    FOnWeightChanged OnWeightChanged;

    UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
    FOnInventoryExpanded OnInventoryExpanded;

    /** Core inventory operations */
    UFUNCTION(BlueprintCallable, Category = "Inventory|Operations")
    bool ExpandInventory(int32 AdditionalSlots);

    UFUNCTION(BlueprintCallable, Category = "Inventory|Operations")
    bool TransferToNearbyContainer();

    /** Weight management */
    UFUNCTION(BlueprintPure, Category = "Inventory|Weight")
    float GetCurrentWeight() const { return CurrentWeight; }

    UFUNCTION(BlueprintPure, Category = "Inventory|Weight")
    float GetMaxWeight() const { return MaxWeight; }

    UFUNCTION(BlueprintPure, Category = "Inventory|Weight")
    float GetWeightPercentage() const;

protected:
    /** Override base container validation */
    virtual bool ValidateItem(const FItemStructure& Item) const override;
    
    /** Additional validation methods */
    bool CheckWeightLimit(const FItemStructure& Item) const;
    bool CheckLevelRequirement(const FItemStructure& Item) const;
    bool IsItemTypeAllowed(const FItemStructure& Item) const;

    /** Network replication */
    UFUNCTION()
    void OnRep_CurrentWeight();

    /** Weight calculations */
    void UpdateCurrentWeight();
    float CalculateItemWeight(const FItemStructure& Item) const;

    /** Internal helpers */
    void InitializeInventory();
    void HandleInventoryExpansion(int32 NewSize);
};