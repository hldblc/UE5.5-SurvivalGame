// Copyright Epic Games, Inc. All Rights Reserved.

#include "MeshIconGeneratorStyle.h"
#include "MeshIconGenerator.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FMeshIconGeneratorStyle::StyleInstance = nullptr;

void FMeshIconGeneratorStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FMeshIconGeneratorStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FMeshIconGeneratorStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("MeshIconGeneratorStyle"));
	return StyleSetName;
}

void FMeshIconGeneratorStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FMeshIconGeneratorStyle::Get()
{
	return *StyleInstance;
}

TSharedRef<FSlateStyleSet> FMeshIconGeneratorStyle::Create()
{
	TSharedRef<FSlateStyleSet> Style = MakeShareable(new FSlateStyleSet("MeshIconGeneratorStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("MeshIconGenerator")->GetBaseDir() / TEXT("Resources"));

	// Icon for the tool button 40x40
	Style->Set("MeshIconGenerator.PluginAction", new IMAGE_BRUSH(TEXT("ButtonIcon_40x"), FVector2D(40.0f, 40.0f)));
	
	// Smaller icon for menu entries 20x20
	Style->Set("MeshIconGenerator.PluginAction.Small", new IMAGE_BRUSH(TEXT("ButtonIcon_40x"), FVector2D(20.0f, 20.0f)));
	
	return Style;
}

#undef RootToContentDir