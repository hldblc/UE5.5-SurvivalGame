// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/SWindow.h"
#include "Engine/StaticMesh.h"
#include "Widgets/Views/STileView.h"
#include "Types/SlateEnums.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SComboBox.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Styling/SlateTypes.h"
#include "Styling/SlateBrush.h"
#include "Engine/World.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/SceneCapture2D.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"

class FMeshIconGeneratorModule;
class SBox;
class SImage;

/**
 * Settings window for Mesh Icon Generator
 */
class SMeshIconGeneratorSettingsWindow : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SMeshIconGeneratorSettingsWindow)
    {}
    SLATE_END_ARGS()

    /** Constructs this widget with InArgs */
    void Construct(const FArguments& InArgs, TSharedRef<SWindow> InParentWindow, FMeshIconGeneratorModule* InModule);
    
    /** Destructor to ensure cleanup */
    virtual ~SMeshIconGeneratorSettingsWindow();

    /** Preview render target */
    UTextureRenderTarget2D* PreviewRenderTarget;

    /** Preview world for rendering */
    UWorld* PreviewWorld = nullptr;
    
    /** Store all preview actors for proper cleanup */
    TArray<AActor*> PreviewActors;
    
    /** Store the scene capture actor for direct cleanup */
    ASceneCapture2D* PreviewCaptureActor = nullptr;
    
    /** Store the mesh component for direct cleanup */
    UStaticMeshComponent* PreviewMeshComponent = nullptr;

    /** Create the preview world - only called once */
    void CreatePreviewWorld();
    
    /** Generate preview for the selected mesh */
    void GeneratePreview();
    
    /** Clear the preview scene without destroying the world */
    void ClearPreviewScene();
    
    /** Clean up the preview world and actors */
    void CleanupPreviewWorld();

    /** Generate icons for all selected meshes with current settings */
    void GenerateIconsWithSettings();
    void RefreshSelectedMeshes();

    /** The window that owns this widget */
    TWeakPtr<SWindow> ParentWindow;

    /** Reference to the plugin module */
    FMeshIconGeneratorModule* PluginModule;

    /** The currently selected static mesh */
    UStaticMesh* SelectedMesh;

    /** The list of selected static meshes */
    TArray<UStaticMesh*> SelectedMeshes;

    /** Image widget for displaying the preview */
    TSharedPtr<SImage> PreviewImage;

    /** Box containing the preview image */
    TSharedPtr<SBox> PreviewBox;

    /** Dynamic brush for preview image */
    TSharedPtr<FSlateDynamicImageBrush> PreviewImageBrush;

    /** Combo box for background color */
    TSharedPtr<SComboBox<TSharedPtr<FString>>> BackgroundTypeComboBox;

    /** Options for background type */
    TArray<TSharedPtr<FString>> BackgroundOptions;

    /** Enum for background options */
    enum class EBackgroundType : uint8
    {
        Transparent,
        SolidColor,
        Gradient,
        Checkerboard
    };

    /** Current background type */
    EBackgroundType CurrentBackgroundType;

    /** Settings for the icon generation */
    struct FMeshIconSettings
    {
        int32 IconSize = 512;
        bool bUseTransparency = true;
        FLinearColor BackgroundColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
        FLinearColor LightColor1 = FLinearColor(1.0f, 0.98f, 0.95f);
        FLinearColor LightColor2 = FLinearColor(0.7f, 0.8f, 1.0f);
        float LightIntensity1 = 20.0f;
        float LightIntensity2 = 8.0f;
        float AmbientLightIntensity = 2.0f;
        float ExposureBias = 0.0f;
        float Contrast = 1.1f;
        float Gamma = 1.1f;
        float Gain = 1.3f;
        bool bInvertAlpha = false;
        float CameraAngle = 45.0f;
        float CameraPitch = -30.0f;
        bool bUseAutoRotation = true;
        bool bShowWireframe = false;
        
        // Manual mesh rotation (only used when bUseAutoRotation is false)
        float MeshYaw = 0.0f;
        float MeshPitch = 0.0f;
        float MeshRoll = 0.0f;
        
        // Light rotation settings
        float LightPitch = -30.0f;
        float LightYaw = 45.0f;
    };

    /** Current settings */
    FMeshIconSettings Settings;

    /** Update preview based on current settings */
    void UpdatePreview();

    /** Callback for when a background type is selected */
    void OnBackgroundTypeSelected(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo);

    /** Get the current background type as readable text */
    FText GetBackgroundTypeAsText() const;

    /** Get function for icon size */
    int32 GetIconSize() const { return Settings.IconSize; }

    /** Set function for icon size */
    void SetIconSize(int32 NewSize);

    /** Commit function for icon size */
    void OnIconSizeCommitted(int32 NewSize, ETextCommit::Type CommitType);

    /** Get the current background color */
    FLinearColor GetBackgroundColor() const { return Settings.BackgroundColor; }

    /** Set the background color */
    void SetBackgroundColor(FLinearColor NewColor);

    /** Save current settings */
    void SaveSettings();

    /** Load saved settings */
    void LoadSettings();

    /** Generate the final icons with current settings */
    FReply OnGenerateClicked();

    /** Cancel and close the window */
    FReply OnCancelClicked();

    /** Helper method to get the selected static mesh */
    UStaticMesh* GetSelectedStaticMesh() const;
    
    /** Default camera distance multiplier */
    float CameraDistance = 2.5f;
    
    /** Whether to show the sky in the preview */
    bool bShowSky = true;
}; 