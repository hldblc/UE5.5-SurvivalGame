// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Items/ItemMaster.h"
#include "SurvivalGame/Public/Interfaces/EquipableItemInterface.h"
#include "Enums/AnimationEnums.h"
#include "EquipableMaster.generated.h"

UCLASS(BlueprintType, Blueprintable)
class SURVIVALGAME_API AEquipableMaster : public AItemMaster, public IEquipableItemInterface
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AEquipableMaster();


    /**
     * Overrides the GetLifetimeReplicatedProps function to define the properties
     * of this actor that will be replicated across the network.
     *
     * @param OutLifetimeProps A reference to an array that will hold all the properties
     *                         that require replication for this actor.
     */
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


    /**
     * Specifies the name of the socket this equipable item should attach to.
     *
     * This property is editable in the editor and accessible in Blueprint scripts.
     * It determines where the equipable item will be placed when equipped, based on
     * the defined socket name.
     *
     * The socket name is specified as a FName and can correspond to predefined
     * sockets in the skeletal mesh of the character or structure to which the item is attached.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Equipable|Socket")
    FName EquipableSocketName;

    /**
     * Represents the current animation state of the equipable item,
     * determining the appropriate animations to play based on the item's use case.
     *
     * The AnimationState can be set or modified in blueprints or via in-game logic
     * to reflect different states such as DefaultState, HatchetState, SwordState, etc.
     *
     * This property is editable in the editor and accessible in Blueprint scripts.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Equipable|AnimationState")
    E_EquipableAnimationStates AnimationState;


    /**
     * Determines if this equipable requires two hands to use.
     * This affects how the item is held and animated.
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category="Equipable|Socket")
    bool bIsTwoHanded;


    /**
     * Specifies the secondary socket name for two-handed items.
     * Only used when bIsTwoHanded is true.
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category="Equipable|TwoHanded", meta=(EditCondition="bIsTwoHanded", EditConditionHides))
    FName TwoHandedSocketName;

    /**
     * Implements the logic to retrieve the equipable item's information,
     * including the socket name, animation state, and two-handed properties.
     *
     * @param OutSocketName A reference to a FName variable where the name
     *                      of the socket this item attaches to will be stored.
     * @param OutEquipableAnimationState A reference to an E_EquipableAnimationStates
     *                                   variable where the current animation state
     *                                   of the equipable item will be stored.
     * @param OutIsTwoHanded A reference to a bool variable that indicates if this
     *                       is a two-handed equipable item.
     * @param OutTwoHandedSocketName A reference to a FName variable where the name
     *                               of the secondary socket will be stored.
     */
    virtual void GetEquipableInfo_Implementation(FName& OutSocketName, 
                                               E_EquipableAnimationStates& OutEquipableAnimationState,
                                               bool& OutIsTwoHanded,
                                               FName& OutTwoHandedSocketName) override;
    
protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
};