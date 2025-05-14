// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;
class SMeshIconGeneratorSettingsWindow;

class FMeshIconGeneratorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command. */
	void PluginButtonClicked();
	
	/** Generate icons for selected static meshes */
	void GenerateIconsForSelectedMeshes();
	
	/** Open settings window for more customization options */
	void OpenSettingsWindow();
	
	/** Reference to the settings window */
	TSharedPtr<SMeshIconGeneratorSettingsWindow> SettingsWindow;
	
	/** Reference to the settings window parent */
	TSharedPtr<SWindow> SettingsWindowParent;
	
private:
	void RegisterMenus();
	TSharedRef<SWidget> GenerateIconSizeMenu();

private:
	TSharedPtr<class FUICommandList> PluginCommands;
	
	/** Settings for icon generation */
	int32 IconSize = 512;
};