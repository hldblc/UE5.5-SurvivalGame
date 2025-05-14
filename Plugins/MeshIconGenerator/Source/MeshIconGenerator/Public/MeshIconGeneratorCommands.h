// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "MeshIconGeneratorStyle.h"

class FMeshIconGeneratorCommands : public TCommands<FMeshIconGeneratorCommands>
{
public:

	FMeshIconGeneratorCommands()
		: TCommands<FMeshIconGeneratorCommands>(TEXT("MeshIconGenerator"), NSLOCTEXT("Contexts", "MeshIconGenerator", "Mesh Icon Generator Plugin"), NAME_None, FMeshIconGeneratorStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};