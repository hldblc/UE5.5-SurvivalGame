// Copyright Epic Games, Inc. All Rights Reserved.

#include "MeshIconGeneratorCommands.h"

#define LOCTEXT_NAMESPACE "FMeshIconGeneratorModule"

void FMeshIconGeneratorCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "Generate Mesh Icons", "Generate thumbnail icons for selected Static Meshes", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control | EModifierKey::Alt, EKeys::M));
}

#undef LOCTEXT_NAMESPACE