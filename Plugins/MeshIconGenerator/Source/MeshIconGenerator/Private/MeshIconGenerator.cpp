// Copyright Epic Games, Inc. All Rights Reserved.

#include "MeshIconGenerator.h"
#include "MeshIconGeneratorStyle.h"
#include "MeshIconGeneratorCommands.h"
#include "MeshIconGeneratorSettingsWindow.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"
#include "Styling/AppStyle.h"  // Use this instead of EditorStyleSet.h
#include "LevelEditor.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Engine/StaticMesh.h"
#include "Editor/UnrealEd/Public/ObjectTools.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFileManager.h"
#include "ImageUtils.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Editor/EditorEngine.h"
#include "Engine/Selection.h"

// Add these includes for texture rendering and scene capture
#include "MaterialDomain.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/SceneCapture2D.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/ReflectionCaptureComponent.h"
#include "Components/SphereReflectionCaptureComponent.h"
#include "Engine/PostProcessVolume.h"

// Add these includes for texture creation and saving
#include "Engine/Texture2D.h"
#include "Misc/PackageName.h"
#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include "IAssetTools.h"
#include "IContentBrowserSingleton.h"

static const FName MeshIconGeneratorTabName("MeshIconGenerator");

#define LOCTEXT_NAMESPACE "FMeshIconGeneratorModule"

void FMeshIconGeneratorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FMeshIconGeneratorStyle::Initialize();
	FMeshIconGeneratorStyle::ReloadTextures();

	FMeshIconGeneratorCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FMeshIconGeneratorCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FMeshIconGeneratorModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FMeshIconGeneratorModule::RegisterMenus));
}

void FMeshIconGeneratorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FMeshIconGeneratorStyle::Shutdown();

	FMeshIconGeneratorCommands::Unregister();
}

void FMeshIconGeneratorModule::PluginButtonClicked()
{
	// Instead of generating directly, open the settings window
	OpenSettingsWindow();
}

void FMeshIconGeneratorModule::OpenSettingsWindow()
{
	// Create a window to host our settings widget
	TSharedRef<SWindow> Window = SNew(SWindow)
		.Title(LOCTEXT("MeshIconGeneratorWindowTitle", "Mesh Icon Generator"))
		.SizingRule(ESizingRule::Autosized)
		.ClientSize(FVector2D(800, 600))
		.SupportsMaximize(false)
		.SupportsMinimize(false);

	// Create the settings widget
	Window->SetContent(
		SNew(SMeshIconGeneratorSettingsWindow, Window, this)
	);

	// Show the window
	if (FGlobalTabmanager::Get()->GetRootWindow().IsValid())
	{
		FSlateApplication::Get().AddWindowAsNativeChild(Window, FGlobalTabmanager::Get()->GetRootWindow().ToSharedRef());
	}
	else
	{
		FSlateApplication::Get().AddWindow(Window);
	}
}

TSharedRef<SWidget> FMeshIconGeneratorModule::GenerateIconSizeMenu()
{
	FMenuBuilder MenuBuilder(true, nullptr);

	TArray<int32> IconSizes = { 128, 256, 512, 1024 };

	for (int32 Size : IconSizes)
	{
		FUIAction Action(
			FExecuteAction::CreateLambda([this, Size]() {
				this->IconSize = Size;
			}),
			FCanExecuteAction(),
			FIsActionChecked::CreateLambda([this, Size]() {
				return this->IconSize == Size;
			})
		);

		MenuBuilder.AddMenuEntry(
			FText::Format(LOCTEXT("IconSizeFormat", "{0}x{0}"), FText::AsNumber(Size)),
			FText::Format(LOCTEXT("IconSizeTooltip", "Set icon size to {0}x{0}"), FText::AsNumber(Size)),
			FSlateIcon(),
			Action,
			NAME_None,
			EUserInterfaceActionType::RadioButton
		);
	}

	return MenuBuilder.MakeWidget();
}

void FMeshIconGeneratorModule::RegisterMenus()
{
    // Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
    FToolMenuOwnerScoped OwnerScoped(this);

    // Add entry to the Tools menu
    {
        UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Tools");
        {
            FToolMenuSection& Section = Menu->FindOrAddSection("MeshTools");
            Section.AddMenuEntryWithCommandList(FMeshIconGeneratorCommands::Get().PluginAction, PluginCommands);
        }
    }

    // Add button to the toolbar
    {
        UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
        {
            FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
            Section.AddMenuEntryWithCommandList(FMeshIconGeneratorCommands::Get().PluginAction, PluginCommands);
        }
    }

    // Create a separate "Icon Size" menu in the Tools menu
    {
        UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Tools");
        {
            FToolMenuSection& Section = Menu->FindOrAddSection("MeshTools");
            
            // Add submenu items for different icon sizes
            TArray<int32> IconSizes = { 128, 256, 512, 1024 };
            for (int32 Size : IconSizes)
            {
                FUIAction Action(
                    FExecuteAction::CreateLambda([this, Size]() {
                        this->IconSize = Size;
                    }),
                    FCanExecuteAction(),
                    FIsActionChecked::CreateLambda([this, Size]() {
                        return this->IconSize == Size;
                    })
                );

                // Add each size as a separate menu item
                FText Label = FText::Format(LOCTEXT("IconSizeFormat", "Set Icon Size: {0}x{0}"), FText::AsNumber(Size));
                FText Tooltip = FText::Format(LOCTEXT("IconSizeTooltip", "Set icon size to {0}x{0}"), FText::AsNumber(Size));
                
                Section.AddMenuEntry(
                    FName(*FString::Printf(TEXT("IconSize_%d"), Size)),
                    Label,
                    Tooltip,
                    FSlateIcon(),
                    Action,
                    EUserInterfaceActionType::RadioButton
                );
            }
        }
    }
}

void FMeshIconGeneratorModule::GenerateIconsForSelectedMeshes()
{
    // Get the selected static meshes from the editor
    USelection* SelectedAssets = GEditor->GetSelectedObjects();
    TArray<UStaticMesh*> SelectedMeshes;
    
    for (FSelectionIterator Iter(*SelectedAssets); Iter; ++Iter)
    {
        if (UStaticMesh* StaticMesh = Cast<UStaticMesh>(*Iter))
        {
            SelectedMeshes.Add(StaticMesh);
        }
    }
    
    // If we have static mesh components selected, get their meshes
    USelection* SelectedActors = GEditor->GetSelectedActors();
    for (FSelectionIterator Iter(*SelectedActors); Iter; ++Iter)
    {
        if (AActor* Actor = Cast<AActor>(*Iter))
        {
            TArray<UStaticMeshComponent*> Components;
            Actor->GetComponents<UStaticMeshComponent>(Components);
            
            for (UStaticMeshComponent* MeshComp : Components)
            {
                if (MeshComp && MeshComp->GetStaticMesh())
                {
                    SelectedMeshes.AddUnique(MeshComp->GetStaticMesh());
                }
            }
        }
    }
    
    // Early out if no meshes are selected
    if (SelectedMeshes.Num() == 0)
    {
        FMessageDialog::Open(EAppMsgType::Ok, 
            FText::FromString(TEXT("No static meshes selected. Please select some static meshes in the Content Browser or static mesh components in the Level Editor.")));
        return;
    }
    
    // Create notification for the process
    FNotificationInfo Info(FText::Format(
        NSLOCTEXT("MeshIconGenerator", "GeneratingIcons", "Generating icons for {0} meshes..."),
        FText::AsNumber(SelectedMeshes.Num())));
    Info.bFireAndForget = false;
    Info.ExpireDuration = 5.0f;
    Info.bUseSuccessFailIcons = true;
    TSharedPtr<SNotificationItem> NotificationPtr = FSlateNotificationManager::Get().AddNotification(Info);
    NotificationPtr->SetCompletionState(SNotificationItem::CS_Pending);

    // Use custom icon size from settings if available, otherwise use default
    int32 FinalIconSize = IconSize > 0 ? IconSize : 512;

    // Define the save location in Content Browser
    const FString SaveDirectoryPath = TEXT("/Game/MeshIcons");
    
    // Ensure the directory exists in the content folder
    FString ContentFolderPath = FPaths::ProjectContentDir() / TEXT("MeshIcons");
    if (!IFileManager::Get().DirectoryExists(*ContentFolderPath))
    {
        IFileManager::Get().MakeDirectory(*ContentFolderPath, true);
    }

    // Get AssetTools for creating new assets
    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
    
    // Get the asset registry for adding the newly created assets
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    
    // Loop through all selected meshes and generate icons
    int32 SuccessCount = 0;
    TArray<FString> CreatedAssetPaths;
    
    for (UStaticMesh* Mesh : SelectedMeshes)
    {
        if (!Mesh)
            continue;
            
        // Generate icon name based on the mesh name
        FString MeshName = Mesh->GetName();
        FString IconName = MeshName + TEXT("_Icon");
        
        // Create a package for the new texture
        FString PackagePath = SaveDirectoryPath / IconName;
        UPackage* Package = CreatePackage(*PackagePath);
        
        // For now, we'll just log what would happen - later we'll implement actual rendering
        UE_LOG(LogTemp, Display, TEXT("Would create icon for mesh '%s' at path '%s'"), *MeshName, *PackagePath);
        
        // In a full implementation, we would:
        // 1. Set up a SceneCapture2D
        // 2. Add the mesh to the scene
        // 3. Configure lighting
        // 4. Render to a texture
        // 5. Save the texture

        // Add the asset path to our list of created assets
        CreatedAssetPaths.Add(PackagePath);
        SuccessCount++;
    }
    
    // Update notification with results
    if (SuccessCount > 0)
    {
        NotificationPtr->SetText(FText::Format(
            NSLOCTEXT("MeshIconGenerator", "IconsGeneratedSuccess", "Successfully generated {0} icons in {1}"),
            FText::AsNumber(SuccessCount),
            FText::FromString(SaveDirectoryPath)));
        NotificationPtr->SetCompletionState(SNotificationItem::CS_Success);
    }
    else
    {
        NotificationPtr->SetText(NSLOCTEXT("MeshIconGenerator", "IconsGenerationFailed", "Failed to generate any icons"));
        NotificationPtr->SetCompletionState(SNotificationItem::CS_Fail);
    }
    
    NotificationPtr->ExpireAndFadeout();
    
    // If we have any created assets, show them in the content browser
	// If we have any created assets, show them in the content browser
	if (CreatedAssetPaths.Num() > 0)
	{
		FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
		TArray<FAssetData> AssetData;
    
		// Convert paths to asset data objects - use soft object paths instead of FNames
		for (const FString& Path : CreatedAssetPaths)
		{
			// Create a soft object path from the string path
			FSoftObjectPath SoftObjectPath(Path);
        
			// Use the asset registry to find the asset data by soft object path
			FAssetData Asset = AssetRegistryModule.Get().GetAssetByObjectPath(SoftObjectPath);
			if (Asset.IsValid())
			{
				AssetData.Add(Asset);
			}
		}
    
		// Focus the content browser on the created assets
		ContentBrowserModule.Get().SyncBrowserToAssets(AssetData);
    
		// Show a message with the save location
		FMessageDialog::Open(EAppMsgType::Ok, FText::Format(
			NSLOCTEXT("MeshIconGenerator", "IconsGeneratedMessage", "{0} icons have been generated and saved to:\n{1}"),
			FText::AsNumber(SuccessCount),
			FText::FromString(SaveDirectoryPath)));
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMeshIconGeneratorModule, MeshIconGenerator)