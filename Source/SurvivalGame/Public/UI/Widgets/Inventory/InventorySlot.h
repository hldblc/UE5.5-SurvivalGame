#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "Struct/ItemStructure.h"
#include "Enums/ContainerType.h"  
#include "Runtime/UMG/Public/Components/TextBlock.h"
#include "CommonUI/Public/CommonBorder.h"
#include "Components/ProgressBar.h"
#include "Input/Events.h"
#include "UI/Widgets/Inventory/DraggedItem.h" // Include DraggedItem.h
#include "InventorySlot.generated.h"

// Forward-declare your item asset type (UItemInfo).
class UItemInfo;
class UItemDrag;

UCLASS(Blueprintable, meta = (DisplayName = "Inventory Slot"))
class SURVIVALGAME_API UInventorySlot : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	UInventorySlot();
	
	// Updates the slot with new item info.
	UFUNCTION(BlueprintCallable, Category="Inventory|Slot")
	void UpdateSlot(FItemStructure ItemInfo);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Slot")
	int32 ItemIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Slot")
	E_ContainerType ContainerType; 

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	// Mouse interaction override for drag and drop functionality
	virtual FReply NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& Operation) override; 
	
	// Widget class to use for drag visual
	UPROPERTY(EditDefaultsOnly, Category = "Drag and Drop")
	TSubclassOf<UDraggedItem> DraggedItemClass;
	
	// Called when the async asset load completes.
	UFUNCTION(BlueprintCallable, Category = "Inventory|Slot")
	void OnItemAssetLoaded();

	// Updates all UI elements based on the loaded item.
	UFUNCTION(BlueprintCallable, Category = "Inventory|Slot")
	void UpdateUIElements() const;

	// Indicates whether this slot currently has an item.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item")
	bool bHasItemInSlot = false;

	// Stores the item info for later reference (and UI updates).
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item")
	FItemStructure StoredItemInfo;

	// The loaded item asset. (Ensure UItemInfo matches your asset type.)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UItemInfo* ItemAssetInfo = nullptr;

	// --- UI Widget Bindings (should match your Blueprint hierarchy) ---

	// The text block that displays the total weight.
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemWeight;

	// The border that shows the item icon.
	UPROPERTY(meta = (BindWidget))
	UCommonBorder* ItemIcon;

	// The text block that shows the item quantity (formatted like "x3").
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemQuantity;

	// A text block that is hidden when an item is present.
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TopText;

	// The text block that shows the ammo information (e.g., "30/30").
	// Renamed to BottomTextAmmo in C++ (make sure your UMG widget is renamed accordingly).
	UPROPERTY(meta = (BindWidget))
	UTextBlock* BottomTextAmmo;

	// The progress bar that shows the item's HP.
	UPROPERTY(meta = (BindWidget))
	UProgressBar* ItemHPBar;
};