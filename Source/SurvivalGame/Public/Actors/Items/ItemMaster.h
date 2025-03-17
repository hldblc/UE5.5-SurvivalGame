// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemMaster.generated.h"

UCLASS()
class SURVIVALGAME_API AItemMaster : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AItemMaster();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


};
