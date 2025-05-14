// Copyright Epic Games, Inc. All Rights Reserved.

#include "MeshIconGeneratorSettingsWindow.h"
#include "MeshIconGenerator.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Images/SImage.h"
#include "PropertyCustomizationHelpers.h"
#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/Colors/SColorPicker.h"
#include "SlateOptMacros.h"
#include "Styling/SlateTypes.h"
#include "Styling/AppStyle.h"  // Use AppStyle for UE5
#include "Styling/SlateBrush.h"
#include "Brushes/SlateDynamicImageBrush.h"
#include "Interfaces/IMainFrameModule.h"
#include "LevelEditor.h"
#include "Components/LightComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SkyLightComponent.h"
#include "Engine/StaticMesh.h"
#include "Editor/UnrealEd/Public/EditorViewportClient.h"
#include "Materials/MaterialInstance.h"
#include "Editor/UnrealEd/Public/Editor.h"
#include "Editor/UnrealEd/Public/Selection.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SceneCapture2D.h"
#include "Engine/SkyLight.h"

#define LOCTEXT_NAMESPACE "MeshIconGeneratorSettings"

SMeshIconGeneratorSettingsWindow::~SMeshIconGeneratorSettingsWindow()
{
    // Make sure we clean up the preview world when the window is closed
    CleanupPreviewWorld();
}

void SMeshIconGeneratorSettingsWindow::Construct(const FArguments& InArgs, TSharedRef<SWindow> InParentWindow, FMeshIconGeneratorModule* InModule)
{
    ParentWindow = InParentWindow;
    PluginModule = InModule;
    
    // Initialize the preview render target
    PreviewRenderTarget = NewObject<UTextureRenderTarget2D>();
    PreviewRenderTarget->InitCustomFormat(256, 256, PF_B8G8R8A8, false);
    PreviewRenderTarget->ClearColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
    PreviewRenderTarget->UpdateResourceImmediate(true);
    PreviewRenderTarget->AddToRoot(); // Keep render target alive
    
    // Create the preview world just once and keep it alive for the entire session
    CreatePreviewWorld();
    
    // Initialize background options
    BackgroundOptions.Add(MakeShareable(new FString(TEXT("Transparent"))));
    BackgroundOptions.Add(MakeShareable(new FString(TEXT("Solid Color"))));
    BackgroundOptions.Add(MakeShareable(new FString(TEXT("Gradient"))));
    BackgroundOptions.Add(MakeShareable(new FString(TEXT("Checkerboard"))));
    
    CurrentBackgroundType = EBackgroundType::Transparent;
    
    // Get currently selected meshes
    USelection* SelectedAssets = GEditor->GetSelectedObjects();
    SelectedMeshes.Empty();
    
    for (FSelectionIterator Iter(*SelectedAssets); Iter; ++Iter)
    {
        if (UStaticMesh* StaticMesh = Cast<UStaticMesh>(*Iter))
        {
            SelectedMeshes.AddUnique(StaticMesh);
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
    
    // Set the selected mesh if we have one
    if (SelectedMeshes.Num() > 0)
    {
        SelectedMesh = SelectedMeshes[0];
    }
    
    // Load settings from config
    LoadSettings();
    
    // Main layout
    ChildSlot
    [
        SNew(SVerticalBox)
        
        // Title
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(FMargin(5.0f, 5.0f))
        [
            SNew(STextBlock)
            .Text(LOCTEXT("WindowTitle", "Mesh Icon Generator Settings"))
            .Font(FAppStyle::Get().GetFontStyle("HeadingFont"))
        ]
        
        // Content
        + SVerticalBox::Slot()
        .FillHeight(1.0f)
        .Padding(FMargin(5.0f, 5.0f))
        [
            SNew(SHorizontalBox)
            
            // Left side - Preview
            + SHorizontalBox::Slot()
            .FillWidth(0.5f)
            .Padding(5.0f)
            [
                SNew(SVerticalBox)
                
                // Preview label
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(0.0f, 0.0f, 0.0f, 5.0f)
                [
                    SNew(STextBlock)
                    .Text(LOCTEXT("PreviewLabel", "Preview"))
                    .Font(FAppStyle::Get().GetFontStyle("NormalFont"))
                    .TextStyle(FAppStyle::Get(), "ContentBrowser.TopBar.Font")
                ]
                
                // Selected mesh
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(0.0f, 0.0f, 0.0f, 5.0f)
                [
                    SNew(SHorizontalBox)
                    
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    .VAlign(VAlign_Center)
                    .Padding(0.0f, 0.0f, 5.0f, 0.0f)
                    [
                        SNew(STextBlock)
                        .Text(LOCTEXT("SelectedMesh", "Selected Mesh:"))
                    ]
                    
                    + SHorizontalBox::Slot()
                    .FillWidth(1.0f)
                    [
                        SNew(SObjectPropertyEntryBox)
                        .AllowedClass(UStaticMesh::StaticClass())
                        .ObjectPath_Lambda([this]() {
                            return SelectedMesh ? SelectedMesh->GetPathName() : FString();
                        })
                        .OnObjectChanged_Lambda([this](const FAssetData& AssetData) {
                            SelectedMesh = Cast<UStaticMesh>(AssetData.GetAsset());
                            if (SelectedMesh && !SelectedMeshes.Contains(SelectedMesh))
                            {
                                SelectedMeshes.AddUnique(SelectedMesh);
                                UE_LOG(LogTemp, Display, TEXT("Added mesh from property box: %s"), 
                                    SelectedMesh ? *SelectedMesh->GetName() : TEXT("None"));
                            }
                            GeneratePreview();
                        })
                    ]
                ]
                
                // Mesh preview
                + SVerticalBox::Slot()
                .FillHeight(1.0f)
                .Padding(0.0f, 0.0f, 0.0f, 5.0f)
                [
                    SAssignNew(PreviewBox, SBox)
                    .WidthOverride(256)
                    .HeightOverride(256)
                    [
                        SAssignNew(PreviewImage, SImage)
                    ]
                ]
                
                // Selected meshes count
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(0.0f, 5.0f, 0.0f, 0.0f)
                [
                    SNew(STextBlock)
                    .Text_Lambda([this]() {
                        return FText::Format(LOCTEXT("SelectedMeshesCount", "Selected Meshes: {0}"), FText::AsNumber(SelectedMeshes.Num()));
                    })
                ]
            ]
            
            // Right side - Settings
            + SHorizontalBox::Slot()
            .FillWidth(0.5f)
            .Padding(5.0f)
            [
                SNew(SScrollBox)
                
                // Icon size
                + SScrollBox::Slot()
                .Padding(0.0f, 0.0f, 0.0f, 5.0f)
                [
                    SNew(SVerticalBox)
                    
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        SNew(STextBlock)
                        .Text(LOCTEXT("IconSizeLabel", "Icon Size"))
                        .Font(FAppStyle::Get().GetFontStyle("NormalFont"))
                        .TextStyle(FAppStyle::Get(), "ContentBrowser.TopBar.Font")
                    ]
                    
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(0.0f, 5.0f, 0.0f, 0.0f)
                    [
                        SNew(SHorizontalBox)
                        
                        + SHorizontalBox::Slot()
                        .FillWidth(1.0f)
                        [
                            SNew(SSpinBox<int32>)
                            .Value(this, &SMeshIconGeneratorSettingsWindow::GetIconSize)
                            .OnValueChanged(this, &SMeshIconGeneratorSettingsWindow::SetIconSize)
                            .OnValueCommitted(this, &SMeshIconGeneratorSettingsWindow::OnIconSizeCommitted)
                            .MinValue(64)
                            .MaxValue(2048)
                            .Delta(64)
                            .ToolTipText(LOCTEXT("IconSizeTooltip", "Size of the generated icon in pixels"))
                        ]
                    ]
                ]
                
                // Background type
                + SScrollBox::Slot()
                .Padding(0.0f, 5.0f, 0.0f, 5.0f)
                [
                    SNew(SVerticalBox)
                    
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        SNew(STextBlock)
                        .Text(LOCTEXT("BackgroundTypeLabel", "Background Type"))
                        .Font(FAppStyle::Get().GetFontStyle("NormalFont"))
                        .TextStyle(FAppStyle::Get(), "ContentBrowser.TopBar.Font")
                    ]
                    
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(0.0f, 5.0f, 0.0f, 0.0f)
                    [
                        SAssignNew(BackgroundTypeComboBox, SComboBox<TSharedPtr<FString>>)
                        .OptionsSource(&BackgroundOptions)
                        .OnSelectionChanged(this, &SMeshIconGeneratorSettingsWindow::OnBackgroundTypeSelected)
                        .OnGenerateWidget_Lambda([](TSharedPtr<FString> Item) {
                            return SNew(STextBlock).Text(FText::FromString(*Item));
                        })
                        .Content()
                        [
                            SNew(STextBlock)
                            .Text(this, &SMeshIconGeneratorSettingsWindow::GetBackgroundTypeAsText)
                        ]
                    ]
                ]
                
                // Background color
                + SScrollBox::Slot()
                .Padding(0.0f, 5.0f, 0.0f, 5.0f)
                [
                    SNew(SVerticalBox)
                    
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        SNew(STextBlock)
                        .Text(LOCTEXT("BackgroundColorLabel", "Background Color"))
                        .Font(FAppStyle::Get().GetFontStyle("NormalFont"))
                        .TextStyle(FAppStyle::Get(), "ContentBrowser.TopBar.Font")
                    ]
                    
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(0.0f, 5.0f, 0.0f, 0.0f)
                    [
                        SNew(SColorBlock)
                        .Color(this, &SMeshIconGeneratorSettingsWindow::GetBackgroundColor)
                        .OnMouseButtonDown_Lambda([this](const FGeometry&, const FPointerEvent&) {
                            FColorPickerArgs PickerArgs;
                            PickerArgs.InitialColor = GetBackgroundColor();
                            PickerArgs.bUseAlpha = true;
                            PickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateSP(this, &SMeshIconGeneratorSettingsWindow::SetBackgroundColor);
                            PickerArgs.ParentWidget = AsShared();
                            OpenColorPicker(PickerArgs);
                            return FReply::Handled();
                        })
                        .Size(FVector2D(40.0f, 20.0f))
                    ]
                ]
                
                // Transparency option
                + SScrollBox::Slot()
                .Padding(0.0f, 5.0f, 0.0f, 5.0f)
                [
                    SNew(SVerticalBox)
                    
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        SNew(SCheckBox)
                        .IsChecked(Settings.bUseTransparency ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
                        .OnCheckStateChanged_Lambda([this](ECheckBoxState NewState) {
                            Settings.bUseTransparency = (NewState == ECheckBoxState::Checked);
                            UpdatePreview();
                        })
                        .Content()
                        [
                            SNew(STextBlock)
                            .Text(LOCTEXT("UseTransparencyLabel", "Use Transparency"))
                        ]
                    ]
                ]
                
                // Invert alpha option
                + SScrollBox::Slot()
                .Padding(0.0f, 5.0f, 0.0f, 5.0f)
                [
                    SNew(SVerticalBox)
                    
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        SNew(SCheckBox)
                        .IsChecked(Settings.bInvertAlpha ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
                        .OnCheckStateChanged_Lambda([this](ECheckBoxState NewState) {
                            Settings.bInvertAlpha = (NewState == ECheckBoxState::Checked);
                            UpdatePreview();
                        })
                        .Content()
                        [
                            SNew(STextBlock)
                            .Text(LOCTEXT("InvertAlphaLabel", "Invert Alpha (1=Mesh, 0=Background)"))
                        ]
                    ]
                ]
                
                // Advanced Settings Section
                + SScrollBox::Slot()
                .Padding(0.0f, 10.0f, 0.0f, 5.0f)
                [
                    SNew(SVerticalBox)
                    
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        SNew(STextBlock)
                        .Text(LOCTEXT("AdvancedSettingsLabel", "Advanced Settings"))
                        .Font(FAppStyle::Get().GetFontStyle("NormalFont"))
                        .TextStyle(FAppStyle::Get(), "ContentBrowser.TopBar.Font")
                    ]
                    
                    // Camera Settings
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(0.0f, 10.0f, 0.0f, 0.0f)
                    [
                        SNew(STextBlock)
                        .Text(LOCTEXT("CameraSettingsLabel", "Camera Settings"))
                        .Font(FAppStyle::Get().GetFontStyle("SmallFont"))
                    ]
                    
                    // Camera FOV
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(10.0f, 5.0f, 0.0f, 0.0f)
                    [
                        SNew(SHorizontalBox)
                        
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign(VAlign_Center)
                        .Padding(0.0f, 0.0f, 5.0f, 0.0f)
                        [
                            SNew(STextBlock)
                            .Text(LOCTEXT("CameraFOVLabel", "FOV:"))
                            .MinDesiredWidth(80.0f)
                        ]
                        
                        + SHorizontalBox::Slot()
                        .FillWidth(1.0f)
                        [
                            SNew(SSpinBox<float>)
                            .Value_Lambda([this]() { return Settings.CameraAngle; })
                            .OnValueChanged_Lambda([this](float NewValue) {
                                Settings.CameraAngle = NewValue;
                                GeneratePreview();
                            })
                            .MinValue(10.0f)
                            .MaxValue(120.0f)
                            .Delta(5.0f)
                        ]
                    ]
                    
                    // Camera Pitch
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(10.0f, 5.0f, 0.0f, 0.0f)
                    [
                        SNew(SHorizontalBox)
                        
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign(VAlign_Center)
                        .Padding(0.0f, 0.0f, 5.0f, 0.0f)
                        [
                            SNew(STextBlock)
                            .Text(LOCTEXT("CameraPitchLabel", "Pitch:"))
                            .MinDesiredWidth(80.0f)
                        ]
                        
                        + SHorizontalBox::Slot()
                        .FillWidth(1.0f)
                        [
                            SNew(SSpinBox<float>)
                            .Value_Lambda([this]() { return Settings.CameraPitch; })
                            .OnValueChanged_Lambda([this](float NewValue) {
                                Settings.CameraPitch = NewValue;
                                GeneratePreview();
                            })
                            .MinValue(-90.0f)
                            .MaxValue(90.0f)
                            .Delta(5.0f)
                        ]
                    ]
                    
                    // Auto Rotation Toggle
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(10.0f, 5.0f, 0.0f, 0.0f)
                    [
                        SNew(SCheckBox)
                        .IsChecked(Settings.bUseAutoRotation ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
                        .OnCheckStateChanged_Lambda([this](ECheckBoxState NewState) {
                            Settings.bUseAutoRotation = (NewState == ECheckBoxState::Checked);
                            GeneratePreview();
                        })
                        .Content()
                        [
                            SNew(STextBlock)
                            .Text(LOCTEXT("AutoRotationLabel", "Auto-rotate based on mesh shape"))
                        ]
                    ]
                    
                    // Manual Mesh Rotation Controls (only enabled when auto-rotation is off)
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(0.0f, 10.0f, 0.0f, 0.0f)
                    [
                        SNew(STextBlock)
                        .Text(LOCTEXT("MeshRotationLabel", "Mesh Rotation"))
                        .Font(FAppStyle::Get().GetFontStyle("SmallFont"))
                        .IsEnabled_Lambda([this]() { return !Settings.bUseAutoRotation; })
                    ]
                    
                    // Rotation - Yaw
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(10.0f, 5.0f, 0.0f, 0.0f)
                    [
                        SNew(SHorizontalBox)
                        .IsEnabled_Lambda([this]() { return !Settings.bUseAutoRotation; })
                        
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign(VAlign_Center)
                        .Padding(0.0f, 0.0f, 5.0f, 0.0f)
                        [
                            SNew(STextBlock)
                            .Text(LOCTEXT("MeshYawLabel", "Yaw:"))
                            .MinDesiredWidth(80.0f)
                        ]
                        
                        + SHorizontalBox::Slot()
                        .FillWidth(1.0f)
                        [
                            SNew(SSpinBox<float>)
                            .Value_Lambda([this]() { return Settings.MeshYaw; })
                            .OnValueChanged_Lambda([this](float NewValue) {
                                Settings.MeshYaw = NewValue;
                                GeneratePreview();
                            })
                            .MinValue(-180.0f)
                            .MaxValue(180.0f)
                            .Delta(15.0f)
                        ]
                    ]
                    
                    // Rotation - Pitch
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(10.0f, 5.0f, 0.0f, 0.0f)
                    [
                        SNew(SHorizontalBox)
                        .IsEnabled_Lambda([this]() { return !Settings.bUseAutoRotation; })
                        
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign(VAlign_Center)
                        .Padding(0.0f, 0.0f, 5.0f, 0.0f)
                        [
                            SNew(STextBlock)
                            .Text(LOCTEXT("MeshPitchLabel", "Pitch:"))
                            .MinDesiredWidth(80.0f)
                        ]
                        
                        + SHorizontalBox::Slot()
                        .FillWidth(1.0f)
                        [
                            SNew(SSpinBox<float>)
                            .Value_Lambda([this]() { return Settings.MeshPitch; })
                            .OnValueChanged_Lambda([this](float NewValue) {
                                Settings.MeshPitch = NewValue;
                                GeneratePreview();
                            })
                            .MinValue(-90.0f)
                            .MaxValue(90.0f)
                            .Delta(15.0f)
                        ]
                    ]
                    
                    // Rotation - Roll
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(10.0f, 5.0f, 0.0f, 0.0f)
                    [
                        SNew(SHorizontalBox)
                        .IsEnabled_Lambda([this]() { return !Settings.bUseAutoRotation; })
                        
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign(VAlign_Center)
                        .Padding(0.0f, 0.0f, 5.0f, 0.0f)
                        [
                            SNew(STextBlock)
                            .Text(LOCTEXT("MeshRollLabel", "Roll:"))
                            .MinDesiredWidth(80.0f)
                        ]
                        
                        + SHorizontalBox::Slot()
                        .FillWidth(1.0f)
                        [
                            SNew(SSpinBox<float>)
                            .Value_Lambda([this]() { return Settings.MeshRoll; })
                            .OnValueChanged_Lambda([this](float NewValue) {
                                Settings.MeshRoll = NewValue;
                                GeneratePreview();
                            })
                            .MinValue(-180.0f)
                            .MaxValue(180.0f)
                            .Delta(15.0f)
                        ]
                    ]
                    
                    // Lighting Settings
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(0.0f, 10.0f, 0.0f, 0.0f)
                    [
                        SNew(STextBlock)
                        .Text(LOCTEXT("LightingSettingsLabel", "Lighting Settings"))
                        .Font(FAppStyle::Get().GetFontStyle("SmallFont"))
                    ]
                    
                    // Key Light Intensity
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(10.0f, 5.0f, 0.0f, 0.0f)
                    [
                        SNew(SHorizontalBox)
                        
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign(VAlign_Center)
                        .Padding(0.0f, 0.0f, 5.0f, 0.0f)
                        [
                            SNew(STextBlock)
                            .Text(LOCTEXT("KeyLightIntensityLabel", "Key Light:"))
                            .MinDesiredWidth(80.0f)
                        ]
                        
                        + SHorizontalBox::Slot()
                        .FillWidth(1.0f)
                        [
                            SNew(SSpinBox<float>)
                            .Value_Lambda([this]() { return Settings.LightIntensity1; })
                            .OnValueChanged_Lambda([this](float NewValue) {
                                Settings.LightIntensity1 = NewValue;
                                GeneratePreview();
                            })
                            .MinValue(0.0f)
                            .MaxValue(50.0f)
                            .Delta(1.0f)
                        ]
                    ]
                    
                    // Fill Light Intensity
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(10.0f, 5.0f, 0.0f, 0.0f)
                    [
                        SNew(SHorizontalBox)
                        
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign(VAlign_Center)
                        .Padding(0.0f, 0.0f, 5.0f, 0.0f)
                        [
                            SNew(STextBlock)
                            .Text(LOCTEXT("FillLightIntensityLabel", "Fill Light:"))
                            .MinDesiredWidth(80.0f)
                        ]
                        
                        + SHorizontalBox::Slot()
                        .FillWidth(1.0f)
                        [
                            SNew(SSpinBox<float>)
                            .Value_Lambda([this]() { return Settings.LightIntensity2; })
                            .OnValueChanged_Lambda([this](float NewValue) {
                                Settings.LightIntensity2 = NewValue;
                                GeneratePreview();
                            })
                            .MinValue(0.0f)
                            .MaxValue(20.0f)
                            .Delta(1.0f)
                        ]
                    ]
                    
                    // Ambient Light Intensity
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(10.0f, 5.0f, 0.0f, 0.0f)
                    [
                        SNew(SHorizontalBox)
                        
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign(VAlign_Center)
                        .Padding(0.0f, 0.0f, 5.0f, 0.0f)
                        [
                            SNew(STextBlock)
                            .Text(LOCTEXT("AmbientLightLabel", "Ambient:"))
                            .MinDesiredWidth(80.0f)
                        ]
                        
                        + SHorizontalBox::Slot()
                        .FillWidth(1.0f)
                        [
                            SNew(SSpinBox<float>)
                            .Value_Lambda([this]() { return Settings.AmbientLightIntensity; })
                            .OnValueChanged_Lambda([this](float NewValue) {
                                Settings.AmbientLightIntensity = NewValue;
                                GeneratePreview();
                            })
                            .MinValue(0.0f)
                            .MaxValue(5.0f)
                            .Delta(0.1f)
                        ]
                    ]
                    
                    // Show Wireframe
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(0.0f, 10.0f, 0.0f, 0.0f)
                    [
                        SNew(SCheckBox)
                        .IsChecked(Settings.bShowWireframe ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
                        .OnCheckStateChanged_Lambda([this](ECheckBoxState NewState) {
                            Settings.bShowWireframe = (NewState == ECheckBoxState::Checked);
                            GeneratePreview();
                        })
                        .Content()
                        [
                            SNew(STextBlock)
                            .Text(LOCTEXT("ShowWireframeLabel", "Show Wireframe"))
                        ]
                    ]
                ]
            ]
        ]
        
        // Buttons
        + SVerticalBox::Slot()
        .AutoHeight()
        .HAlign(HAlign_Right)
        .Padding(FMargin(5.0f, 5.0f))
        [
            SNew(SHorizontalBox)
            
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(5.0f, 0.0f, 0.0f, 0.0f)
            [
                SNew(SButton)
                .Text(LOCTEXT("GenerateButton", "Generate"))
                .OnClicked(this, &SMeshIconGeneratorSettingsWindow::OnGenerateClicked)
            ]
            
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(5.0f, 0.0f, 0.0f, 0.0f)
            [
                SNew(SButton)
                .Text(LOCTEXT("CancelButton", "Cancel"))
                .OnClicked(this, &SMeshIconGeneratorSettingsWindow::OnCancelClicked)
            ]
        ]
    ];
    
    // Call RefreshSelectedMeshes to ensure proper updating of the selection
    RefreshSelectedMeshes();
    
    // Generate initial preview
    if (SelectedMesh)
    {
        GeneratePreview();
    }
}

void SMeshIconGeneratorSettingsWindow::CreatePreviewWorld()
{
    // Only create the world if it doesn't exist
    if (PreviewWorld)
    {
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("Creating new Preview World for Mesh Icon Generator"));
    
    // Create a new preview world
    PreviewWorld = UWorld::CreateWorld(EWorldType::EditorPreview, false);
    if (!PreviewWorld)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create preview world for Mesh Icon Generator"));
        return;
    }

    // Create the render target
    if (!PreviewRenderTarget)
    {
        PreviewRenderTarget = NewObject<UTextureRenderTarget2D>();
        if (PreviewRenderTarget)
        {
            PreviewRenderTarget->InitCustomFormat(1024, 1024, EPixelFormat::PF_B8G8R8A8, true);
            PreviewRenderTarget->UpdateResourceImmediate(true);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create render target for Mesh Icon Generator"));
        }
    }
}

void SMeshIconGeneratorSettingsWindow::RefreshSelectedMeshes()
{
    // Log for debugging
    UE_LOG(LogTemp, Display, TEXT("Refreshing selected meshes..."));
    
    // Clear existing selection
    SelectedMeshes.Empty();
    
    // Get currently selected meshes from content browser
    USelection* SelectedAssets = GEditor->GetSelectedObjects();
    if (SelectedAssets)
    {
        for (FSelectionIterator Iter(*SelectedAssets); Iter; ++Iter)
        {
            if (UStaticMesh* StaticMesh = Cast<UStaticMesh>(*Iter))
            {
                SelectedMeshes.AddUnique(StaticMesh);
                UE_LOG(LogTemp, Display, TEXT("Found static mesh in content browser: %s"), *StaticMesh->GetName());
            }
        }
    }
    
    // Get meshes from selected actors
    USelection* SelectedActors = GEditor->GetSelectedActors();
    if (SelectedActors)
    {
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
                        UE_LOG(LogTemp, Display, TEXT("Found static mesh component: %s"), *MeshComp->GetStaticMesh()->GetName());
                    }
                }
            }
        }
    }
    
    // Log the total count
    UE_LOG(LogTemp, Display, TEXT("Total selected meshes: %d"), SelectedMeshes.Num());
    
    // Update the selected mesh if we have at least one
    if (SelectedMeshes.Num() > 0 && !SelectedMesh)
    {
        SelectedMesh = SelectedMeshes[0];
        UE_LOG(LogTemp, Display, TEXT("Setting selected mesh to: %s"), *SelectedMesh->GetName());
        GeneratePreview();
    }
}

void SMeshIconGeneratorSettingsWindow::GeneratePreview()
{
    // Create the world if it doesn't exist yet
    if (!PreviewWorld)
    {
        CreatePreviewWorld();
    }
    
    // Clear previous preview scene contents
    ClearPreviewScene();
    
    if (!PreviewWorld || !PreviewRenderTarget)
    {
        UE_LOG(LogTemp, Error, TEXT("Preview world or render target is null in GeneratePreview"));
        return;
    }

    // Get the selected static mesh - Changed variable name to avoid hiding class member
    UStaticMesh* CurrentMesh = GetSelectedStaticMesh();
    if (!CurrentMesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("No static mesh selected for preview generation"));
        return;
    }

    // Spawn a scene capture actor
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    PreviewCaptureActor = PreviewWorld->SpawnActor<ASceneCapture2D>(SpawnParams);
    
    if (!PreviewCaptureActor)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to spawn scene capture actor"));
        return;
    }
    
    // Add to our list for tracking
    PreviewActors.Add(PreviewCaptureActor);

    // Configure the scene capture component
    USceneCaptureComponent2D* CaptureComponent = PreviewCaptureActor->GetCaptureComponent2D();
    if (!CaptureComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("Scene capture component is null"));
        return;
    }

    CaptureComponent->TextureTarget = PreviewRenderTarget;
    CaptureComponent->CaptureSource = SCS_FinalColorLDR;
    CaptureComponent->bEnableClipPlane = false;
    CaptureComponent->bCaptureEveryFrame = true;
    CaptureComponent->bCaptureOnMovement = true;
    CaptureComponent->bAlwaysPersistRenderingState = true;
    
    // Set the capture component properties based on the user settings
    CaptureComponent->ShowFlags.SetTemporalAA(true);
    CaptureComponent->ShowFlags.SetMotionBlur(false);
    CaptureComponent->ShowFlags.SetLighting(true);
    CaptureComponent->ShowFlags.SetPostProcessing(true);
    
    // Use the class member for sky visibility
    CaptureComponent->ShowFlags.SetAtmosphere(bShowSky);
    CaptureComponent->ShowFlags.SetAntiAliasing(true);
    
    // Set HDR-specific properties
    CaptureComponent->ShowFlags.SetTonemapper(true);
    
    // Enable wireframe display if selected
    if (Settings.bShowWireframe)
    {
        CaptureComponent->ShowFlags.SetWireframe(true);
    }
    
    // Spawn a static mesh actor for the selected mesh
    AActor* StaticMeshActor = PreviewWorld->SpawnActor<AActor>(SpawnParams);
    if (!StaticMeshActor)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to spawn static mesh actor"));
        return;
    }
    
    // Add to our list for tracking
    PreviewActors.Add(StaticMeshActor);
    
    // Create and attach a static mesh component
    PreviewMeshComponent = NewObject<UStaticMeshComponent>(StaticMeshActor);
    if (!PreviewMeshComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create static mesh component"));
        return;
    }
    
    PreviewMeshComponent->SetStaticMesh(CurrentMesh);
    PreviewMeshComponent->SetMobility(EComponentMobility::Movable);
    PreviewMeshComponent->RegisterComponent();
    StaticMeshActor->SetRootComponent(PreviewMeshComponent);
    
    // Apply mesh rotation (either auto or manual)
    if (Settings.bUseAutoRotation)
    {
        // Calculate the appropriate rotation to center the mesh in the view
        // The auto rotation can be calculated based on the mesh bounds
        FBoxSphereBounds Bounds = CurrentMesh->GetBounds();
        float BoundsSize = Bounds.SphereRadius * 2.0f;
        
        // Center the mesh
        PreviewMeshComponent->SetRelativeLocation(FVector::ZeroVector);
        
        // Apply a standard rotation that looks good for most meshes
        PreviewMeshComponent->SetRelativeRotation(FRotator(-20.0f, 45.0f, 0.0f));
    }
    else
    {
        // Use manual rotation values
        FRotator MeshRotation(
            Settings.MeshPitch, // Pitch
            Settings.MeshYaw,   // Yaw
            Settings.MeshRoll   // Roll
        );
        
        UE_LOG(LogTemp, Warning, TEXT("Applying manual rotation: Pitch=%f, Yaw=%f, Roll=%f"), 
            Settings.MeshPitch, Settings.MeshYaw, Settings.MeshRoll);
            
        // Center the mesh
        PreviewMeshComponent->SetRelativeLocation(FVector::ZeroVector);
        
        // Apply manual rotation
        PreviewMeshComponent->SetRelativeRotation(MeshRotation);
    }
    
    // Create a directional light
    AActor* DirectionalLightActor = PreviewWorld->SpawnActor<AActor>(SpawnParams);
    if (!DirectionalLightActor)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to spawn directional light actor"));
        return;
    }
    
    // Add to our list for tracking
    PreviewActors.Add(DirectionalLightActor);
    
    UDirectionalLightComponent* DirectionalLightComponent = NewObject<UDirectionalLightComponent>(DirectionalLightActor);
    if (!DirectionalLightComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create directional light component"));
        return;
    }
    
    DirectionalLightComponent->RegisterComponent();
    DirectionalLightActor->SetRootComponent(DirectionalLightComponent);
    
    // Configure the directional light
    DirectionalLightComponent->SetIntensity(Settings.LightIntensity1);
    DirectionalLightComponent->SetLightColor(Settings.LightColor1);
    
    // Use the settings light rotation values
    DirectionalLightComponent->SetRelativeRotation(
        FRotator(Settings.LightPitch, Settings.LightYaw, 0.0f)
    );
    
    // Add a skylight if sky is enabled
    if (bShowSky)
    {
        AActor* SkyLightActor = PreviewWorld->SpawnActor<AActor>(SpawnParams);
        if (SkyLightActor)
        {
            PreviewActors.Add(SkyLightActor);
            
            USkyLightComponent* SkyLightComponent = NewObject<USkyLightComponent>(SkyLightActor);
            if (SkyLightComponent)
            {
                SkyLightComponent->RegisterComponent();
                SkyLightActor->SetRootComponent(SkyLightComponent);
                SkyLightComponent->SetIntensity(Settings.AmbientLightIntensity);
                SkyLightComponent->SourceType = ESkyLightSourceType::SLS_CapturedScene;
                SkyLightComponent->RecaptureSky();
            }
        }
    }
    
    // Position the scene capture to frame the mesh properly
    FBoxSphereBounds Bounds = CurrentMesh->GetBounds();
    float BoundsSize = Bounds.SphereRadius * 2.0f;
    
    // Adjust based on camera distance setting using the class member
    float Distance = FMath::Max<float>(BoundsSize * CameraDistance, 10.0f);
    
    // Position camera based on camera settings
    PreviewCaptureActor->SetActorLocation(FVector(0, 0, 0) - FVector(Distance, 0, 0));
    PreviewCaptureActor->SetActorRotation(FRotator(0, 0, 0));
    
    // Set camera FOV
    CaptureComponent->FOVAngle = Settings.CameraAngle;
    
    // Request one capture
    CaptureComponent->CaptureScene();
    
    // Refresh the icon preview
    if (PreviewImage.IsValid())
    {
        // Create a new brush with the render target
        FSlateBrush Brush;
        Brush.SetResourceObject(PreviewRenderTarget);
        Brush.SetImageSize(FVector2D(256.0f, 256.0f));
        
        // Use SetImage instead of SetBrush
        PreviewImage->SetImage(&Brush);
    }
    
    // Flush the rendering commands to ensure the preview is updated
    FlushRenderingCommands();
}

void SMeshIconGeneratorSettingsWindow::ClearPreviewScene()
{
    if (!PreviewWorld)
    {
        return;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("Clearing Preview Scene (actors count: %d)"), PreviewActors.Num());
    
    // Reset tracked components
    PreviewMeshComponent = nullptr;
    PreviewCaptureActor = nullptr;
    
    // Safely remove all actors we created
    for (AActor* Actor : PreviewActors)
    {
        if (Actor)
        {
            // Unregister all components first
            TSet<UActorComponent*> Components = Actor->GetComponents();
            for (UActorComponent* Component : Components)
            {
                if (Component && Component->IsRegistered())
                {
                    Component->UnregisterComponent();
                }
            }
            
            // Now destroy the actor
            Actor->Destroy();
        }
    }
    
    // Clear the tracking array
    PreviewActors.Empty();
    
    // Flush rendering commands to ensure everything is cleaned up
    FlushRenderingCommands();
}

void SMeshIconGeneratorSettingsWindow::CleanupPreviewWorld()
{
    static bool bIsCleaningUp = false;
    
    // Guard against recursive calls
    if (bIsCleaningUp)
    {
        return;
    }
    
    bIsCleaningUp = true;
    
    UE_LOG(LogTemp, Warning, TEXT("Cleaning up Preview World"));
    
    // Clear the scene first
    ClearPreviewScene();
    
    // Clean up the render target
    if (PreviewRenderTarget)
    {
        PreviewRenderTarget->ReleaseResource();
        PreviewRenderTarget = nullptr;
    }
    
    // Destroy the preview world - use proper UE world destruction API
    if (PreviewWorld != nullptr)
    {
        FlushRenderingCommands();
        
        // The correct way to destroy a world in UE5
        // Make sure to remove it from the root set if it was added
        if (PreviewWorld->IsRooted())
        {
            PreviewWorld->RemoveFromRoot();
        }
        
        // Get the engine subsystem
        if (GEngine)
        {
            // Find the world's context
            FWorldContext* WorldContext = GEngine->GetWorldContextFromWorld(PreviewWorld);
            if (WorldContext)
            {
                // Let the engine handle world destruction properly
                GEngine->DestroyWorldContext(PreviewWorld);
            }
        }
        
        // Set to null after cleanup
        PreviewWorld = nullptr;
    }
    
    bIsCleaningUp = false;
}

void SMeshIconGeneratorSettingsWindow::UpdatePreview()
{
    // Basic update until we implement real preview generation
    if (!PreviewImageBrush.IsValid())
    {
        // Create a texture resource for the render target
        FName BrushName = MakeUniqueObjectName(nullptr, UTexture2D::StaticClass(), FName("MeshIconPreview"));
        PreviewImageBrush = MakeShareable(new FSlateDynamicImageBrush(
            BrushName,
            FVector2D(256, 256)
        ));
    
        // Set the resource object to our render target
        if (PreviewRenderTarget)
        {
            PreviewImageBrush->SetResourceObject(PreviewRenderTarget);
        }
    }
    else
    {
        // Update existing brush with new render target
        if (PreviewRenderTarget)
        {
            PreviewImageBrush->SetResourceObject(PreviewRenderTarget);
        }
    }
    
    // Set the image
    if (PreviewImage.IsValid() && PreviewImageBrush.IsValid())
    {
        // The FSlateDynamicImageBrush inherits from FSlateBrush, so we can use it directly
        const FSlateBrush* Brush = PreviewImageBrush.Get();
        PreviewImage->SetImage(Brush);
    }
}

void SMeshIconGeneratorSettingsWindow::SetIconSize(int32 NewSize)
{
    Settings.IconSize = NewSize;
    // Update the preview after changing the size
    UpdatePreview();
}

void SMeshIconGeneratorSettingsWindow::OnIconSizeCommitted(int32 NewSize, ETextCommit::Type CommitType)
{
    Settings.IconSize = NewSize;
    // Update the preview after changing the size
    UpdatePreview();
}

void SMeshIconGeneratorSettingsWindow::SetBackgroundColor(FLinearColor NewColor)
{
    Settings.BackgroundColor = NewColor;
    UpdatePreview();
}

void SMeshIconGeneratorSettingsWindow::OnBackgroundTypeSelected(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo)
{
    if (NewSelection.IsValid())
    {
        if (*NewSelection == TEXT("Transparent"))
        {
            CurrentBackgroundType = EBackgroundType::Transparent;
            Settings.BackgroundColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
        }
        else if (*NewSelection == TEXT("Solid Color"))
        {
            CurrentBackgroundType = EBackgroundType::SolidColor;
            // Keep existing color but ensure alpha is set
            Settings.BackgroundColor.A = 1.0f;
        }
        else if (*NewSelection == TEXT("Gradient"))
        {
            CurrentBackgroundType = EBackgroundType::Gradient;
            // Keep existing color but ensure alpha is set
            Settings.BackgroundColor.A = 1.0f;
        }
        else if (*NewSelection == TEXT("Checkerboard"))
        {
            CurrentBackgroundType = EBackgroundType::Checkerboard;
            // Keep existing color but ensure alpha is set
            Settings.BackgroundColor.A = 1.0f;
        }
        
        UpdatePreview();
    }
}

FText SMeshIconGeneratorSettingsWindow::GetBackgroundTypeAsText() const
{
    switch (CurrentBackgroundType)
    {
        case EBackgroundType::Transparent:
            return LOCTEXT("TransparentBackground", "Transparent");
        case EBackgroundType::SolidColor:
            return LOCTEXT("SolidColorBackground", "Solid Color");
        case EBackgroundType::Gradient:
            return LOCTEXT("GradientBackground", "Gradient");
        case EBackgroundType::Checkerboard:
            return LOCTEXT("CheckerboardBackground", "Checkerboard");
        default:
            return LOCTEXT("UnknownBackground", "Unknown");
    }
}

void SMeshIconGeneratorSettingsWindow::LoadSettings()
{
    // Load settings from GConfig
    FString ConfigSection = TEXT("MeshIconGenerator");
    
    int32 LoadedIconSize;
    if (GConfig->GetInt(*ConfigSection, TEXT("IconSize"), LoadedIconSize, GEditorPerProjectIni))
    {
        Settings.IconSize = LoadedIconSize;
    }
    
    bool bLoadedUseTransparency;
    if (GConfig->GetBool(*ConfigSection, TEXT("UseTransparency"), bLoadedUseTransparency, GEditorPerProjectIni))
    {
        Settings.bUseTransparency = bLoadedUseTransparency;
    }
    
    bool bLoadedInvertAlpha;
    if (GConfig->GetBool(*ConfigSection, TEXT("InvertAlpha"), bLoadedInvertAlpha, GEditorPerProjectIni))
    {
        Settings.bInvertAlpha = bLoadedInvertAlpha;
    }
    
    // Load auto-rotation setting
    bool bLoadedUseAutoRotation;
    if (GConfig->GetBool(*ConfigSection, TEXT("UseAutoRotation"), bLoadedUseAutoRotation, GEditorPerProjectIni))
    {
        Settings.bUseAutoRotation = bLoadedUseAutoRotation;
    }
    
    // Load wireframe setting
    bool bLoadedShowWireframe;
    if (GConfig->GetBool(*ConfigSection, TEXT("ShowWireframe"), bLoadedShowWireframe, GEditorPerProjectIni))
    {
        Settings.bShowWireframe = bLoadedShowWireframe;
    }
    
    // Load manual mesh rotation settings
    float LoadedMeshYaw, LoadedMeshPitch, LoadedMeshRoll;
    if (GConfig->GetFloat(*ConfigSection, TEXT("MeshYaw"), LoadedMeshYaw, GEditorPerProjectIni))
    {
        Settings.MeshYaw = LoadedMeshYaw;
    }
    if (GConfig->GetFloat(*ConfigSection, TEXT("MeshPitch"), LoadedMeshPitch, GEditorPerProjectIni))
    {
        Settings.MeshPitch = LoadedMeshPitch;
    }
    if (GConfig->GetFloat(*ConfigSection, TEXT("MeshRoll"), LoadedMeshRoll, GEditorPerProjectIni))
    {
        Settings.MeshRoll = LoadedMeshRoll;
    }
    
    // Load camera settings
    float LoadedCameraAngle, LoadedCameraPitch;
    if (GConfig->GetFloat(*ConfigSection, TEXT("CameraAngle"), LoadedCameraAngle, GEditorPerProjectIni))
    {
        Settings.CameraAngle = LoadedCameraAngle;
    }
    if (GConfig->GetFloat(*ConfigSection, TEXT("CameraPitch"), LoadedCameraPitch, GEditorPerProjectIni))
    {
        Settings.CameraPitch = LoadedCameraPitch;
    }
    
    // Load lighting settings
    float LoadedLightIntensity1, LoadedLightIntensity2, LoadedAmbientLightIntensity;
    if (GConfig->GetFloat(*ConfigSection, TEXT("LightIntensity1"), LoadedLightIntensity1, GEditorPerProjectIni))
    {
        Settings.LightIntensity1 = LoadedLightIntensity1;
    }
    if (GConfig->GetFloat(*ConfigSection, TEXT("LightIntensity2"), LoadedLightIntensity2, GEditorPerProjectIni))
    {
        Settings.LightIntensity2 = LoadedLightIntensity2;
    }
    if (GConfig->GetFloat(*ConfigSection, TEXT("AmbientLightIntensity"), LoadedAmbientLightIntensity, GEditorPerProjectIni))
    {
        Settings.AmbientLightIntensity = LoadedAmbientLightIntensity;
    }
    
    FString BackgroundTypeStr;
    if (GConfig->GetString(*ConfigSection, TEXT("BackgroundType"), BackgroundTypeStr, GEditorPerProjectIni))
    {
        if (BackgroundTypeStr == TEXT("Transparent"))
        {
            CurrentBackgroundType = EBackgroundType::Transparent;
        }
        else if (BackgroundTypeStr == TEXT("SolidColor"))
        {
            CurrentBackgroundType = EBackgroundType::SolidColor;
        }
        else if (BackgroundTypeStr == TEXT("Gradient"))
        {
            CurrentBackgroundType = EBackgroundType::Gradient;
        }
        else if (BackgroundTypeStr == TEXT("Checkerboard"))
        {
            CurrentBackgroundType = EBackgroundType::Checkerboard;
        }
    }
    
    FVector BackgroundColorRGB;
    float BackgroundColorA;
    if (GConfig->GetVector(*ConfigSection, TEXT("BackgroundColorRGB"), BackgroundColorRGB, GEditorPerProjectIni) &&
        GConfig->GetFloat(*ConfigSection, TEXT("BackgroundColorA"), BackgroundColorA, GEditorPerProjectIni))
    {
        Settings.BackgroundColor = FLinearColor(BackgroundColorRGB.X, BackgroundColorRGB.Y, BackgroundColorRGB.Z, BackgroundColorA);
    }
}

void SMeshIconGeneratorSettingsWindow::SaveSettings()
{
    // Save settings to GConfig
    FString ConfigSection = TEXT("MeshIconGenerator");
    
    GConfig->SetInt(*ConfigSection, TEXT("IconSize"), Settings.IconSize, GEditorPerProjectIni);
    GConfig->SetBool(*ConfigSection, TEXT("UseTransparency"), Settings.bUseTransparency, GEditorPerProjectIni);
    GConfig->SetBool(*ConfigSection, TEXT("InvertAlpha"), Settings.bInvertAlpha, GEditorPerProjectIni);
    
    // Save auto-rotation and wireframe settings
    GConfig->SetBool(*ConfigSection, TEXT("UseAutoRotation"), Settings.bUseAutoRotation, GEditorPerProjectIni);
    GConfig->SetBool(*ConfigSection, TEXT("ShowWireframe"), Settings.bShowWireframe, GEditorPerProjectIni);
    
    // Save manual mesh rotation settings
    GConfig->SetFloat(*ConfigSection, TEXT("MeshYaw"), Settings.MeshYaw, GEditorPerProjectIni);
    GConfig->SetFloat(*ConfigSection, TEXT("MeshPitch"), Settings.MeshPitch, GEditorPerProjectIni);
    GConfig->SetFloat(*ConfigSection, TEXT("MeshRoll"), Settings.MeshRoll, GEditorPerProjectIni);
    
    // Save camera settings
    GConfig->SetFloat(*ConfigSection, TEXT("CameraAngle"), Settings.CameraAngle, GEditorPerProjectIni);
    GConfig->SetFloat(*ConfigSection, TEXT("CameraPitch"), Settings.CameraPitch, GEditorPerProjectIni);
    
    // Save lighting settings
    GConfig->SetFloat(*ConfigSection, TEXT("LightIntensity1"), Settings.LightIntensity1, GEditorPerProjectIni);
    GConfig->SetFloat(*ConfigSection, TEXT("LightIntensity2"), Settings.LightIntensity2, GEditorPerProjectIni);
    GConfig->SetFloat(*ConfigSection, TEXT("AmbientLightIntensity"), Settings.AmbientLightIntensity, GEditorPerProjectIni);
    
    FString BackgroundTypeStr;
    switch (CurrentBackgroundType)
    {
        case EBackgroundType::Transparent:
            BackgroundTypeStr = TEXT("Transparent");
            break;
        case EBackgroundType::SolidColor:
            BackgroundTypeStr = TEXT("SolidColor");
            break;
        case EBackgroundType::Gradient:
            BackgroundTypeStr = TEXT("Gradient");
            break;
        case EBackgroundType::Checkerboard:
            BackgroundTypeStr = TEXT("Checkerboard");
            break;
        default:
            BackgroundTypeStr = TEXT("Transparent");
            break;
    }
    
    GConfig->SetString(*ConfigSection, TEXT("BackgroundType"), *BackgroundTypeStr, GEditorPerProjectIni);
    
    FVector BackgroundColorRGB(Settings.BackgroundColor.R, Settings.BackgroundColor.G, Settings.BackgroundColor.B);
    GConfig->SetVector(*ConfigSection, TEXT("BackgroundColorRGB"), BackgroundColorRGB, GEditorPerProjectIni);
    GConfig->SetFloat(*ConfigSection, TEXT("BackgroundColorA"), Settings.BackgroundColor.A, GEditorPerProjectIni);
    
    // Save the config file
    GConfig->Flush(false, GEditorPerProjectIni);
}

FReply SMeshIconGeneratorSettingsWindow::OnGenerateClicked()
{
    // Save settings before generating
    SaveSettings();
    
    // Generate icons with current settings
    GenerateIconsWithSettings();
    
    // Close the window
    if (ParentWindow.IsValid())
    {
        ParentWindow.Pin()->RequestDestroyWindow();
    }
    
    return FReply::Handled();
}

FReply SMeshIconGeneratorSettingsWindow::OnCancelClicked()
{
    // Close the window without generating
    if (ParentWindow.IsValid())
    {
        ParentWindow.Pin()->RequestDestroyWindow();
    }
    
    return FReply::Handled();
}

void SMeshIconGeneratorSettingsWindow::GenerateIconsWithSettings()
{
    // We'll call the plugin module's function to generate icons with our settings
    if (PluginModule && SelectedMeshes.Num() > 0)
    {
        // For now, just call the existing function
        // We'll update this later to pass our settings
        PluginModule->GenerateIconsForSelectedMeshes();
    }
}

// Add this helper method to get the selected static mesh
UStaticMesh* SMeshIconGeneratorSettingsWindow::GetSelectedStaticMesh() const
{
    return SelectedMesh;
}

#undef LOCTEXT_NAMESPACE