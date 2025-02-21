#pragma once

#include "CoreMinimal.h"
#include "Enums/ContainerType.h"
#include "Data/Struct/ItemStructure.h"
#include "GameFramework/PlayerController.h"
#include "UI/Widgets/DefaultHUDLayout.h"
#include "InputMappingContext.h"
#include "Engine/AssetManager.h"
#include "Engine/ObjectLibrary.h"
#include "PrimaryData/ItemInfo.h"
#include "UObject/SoftObjectPtr.h"
#include "SurvivalGame/Public/Interfaces/ControllerInterface.h"

// Forward declarations
class UInputAction;
class UMasterUILayout;
class UGameInventoryLayout;  // Add this forward declaration
class UInventorySlot;

#include "SurvivalPlayerController.generated.h"

/**
 * @brief Player controller for the survival game.
 * Handles inventory toggling and implements the CloseInventory function via the ControllerInterface.
 */
UCLASS(Blueprintable, BlueprintType)
class SURVIVALGAME_API ASurvivalPlayerController : public APlayerController, public IControllerInterface
{
    GENERATED_BODY()

public:
    // Constructor.
    ASurvivalPlayerController();

    UFUNCTION(BlueprintCallable, Category = "Debug")
    void DebugListAllItemAssets()
    {
        UAssetManager* AssetManager = UAssetManager::GetIfInitialized();
        if (!AssetManager)
        {
            UE_LOG(LogTemp, Error, TEXT("AssetManager not initialized!"));
            return;
        }
    
        UE_LOG(LogTemp, Log, TEXT("=== ALL AVAILABLE ITEM ASSETS ==="));
    
        // Try to list all UItemInfo assets no matter where they are
        UObjectLibrary* ItemLibrary = UObjectLibrary::CreateLibrary(UItemInfo::StaticClass(), true, true);
        ItemLibrary->LoadAssetDataFromPath("/Game");
    
        TArray<FAssetData> AllItems;
        ItemLibrary->GetAssetDataList(AllItems);
    
        UE_LOG(LogTemp, Log, TEXT("ObjectLibrary found %d UItemInfo assets:"), AllItems.Num());
        for (const FAssetData& Asset : AllItems)
        {
            UE_LOG(LogTemp, Log, TEXT("  Path: %s"), *Asset.GetSoftObjectPath().ToString());
        
            // Try to load it and check its registry key
            UItemInfo* Item = Cast<UItemInfo>(Asset.GetAsset());
            if (Item)
            {
                UE_LOG(LogTemp, Log, TEXT("    RegistryKey: %s"), *Item->RegistryKey.ToString());
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("    Failed to load asset"));
            }
        }
    }

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    /** Input action to toggle the inventory (configured in Editor defaults). */
    UPROPERTY(EditDefaultsOnly, Category = "Input|Actions")
    TObjectPtr<UInputAction> IA_InventoryToggle;
    
    /** Default input mapping context from CommonUI input settings. */
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputMappingContext> DefaultInputMapping;

    /** Class reference for the Master UI Layout widget (set in the Editor). */
    UPROPERTY(EditDefaultsOnly, Category="UI|Class Properties")
    TSubclassOf<UMasterUILayout> MasterLayoutClass;

    /** Reference to our Master UI Layout instance that organizes our CommonUI stacks. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI|References")
    TObjectPtr<UMasterUILayout> RootLayout;

    /** Flag indicating whether the inventory is currently shown. */
    UPROPERTY(Transient)
    bool bInventoryShown{false};

    /** Cached reference to the game inventory layout */
    UPROPERTY()
    TObjectPtr<UGameInventoryLayout> CachedGameInventoryLayout;

    /** 
     * Client function to toggle the inventory UI.
     * When the inventory is open, pressing the toggle will close it.
     */
    UFUNCTION(Client, Reliable)
    void InventoryOnClient();
    void InventoryOnClient_Implementation();

    /**
     * Implementation of the ControllerInterface function to close the inventory.
     * You can override this in Blueprint if you want to add extra CommonUI polish.
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
    void CloseInventory();
    virtual void CloseInventory_Implementation() override;

    UFUNCTION(BlueprintCallable,BlueprintType, Category = "Inventory")
    void InitializeInventoryWidget();
    
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    UInventorySlot* GetInventorySlotWidget(E_ContainerType ContainerType, int32 SlotIndex);
    
    UFUNCTION(Client, Reliable, Category = "Inventory")
    void Client_UpdateSlot(E_ContainerType Container, FItemStructure ItemInfo, int32 Index);
    
    virtual void UpdateItemSlot_Implementation(E_ContainerType ContainerType, FItemStructure ItemInfo, int32 Index) override;
    
private:
    /** Helper function to initialize our CommonUI-enhanced input mappings. */
    void InitializeEnhancedInput() const;

    /** Helper function to create and add our Master UI Layout widget. */
    void CreateMasterLayout();
};