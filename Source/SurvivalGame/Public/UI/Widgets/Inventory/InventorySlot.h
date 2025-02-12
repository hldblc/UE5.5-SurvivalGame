#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "Struct/ItemStructure.h"
#include "Enums/ContainerType.h"  
#include "Runtime/UMG/Public/Components/TextBlock.h"
#include "CommonUI/Public/CommonBorder.h"
#include "Components/ProgressBar.h"
#include "InventorySlot.generated.h"

// Forward-declare your item asset type (UItemInfo).
class UItemInfo;

UCLASS(Blueprintable, meta = (DisplayName = "Inventory Slot"))
class SURVIVALGAME_API UInventorySlot : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	// Updates the slot with new item info.
	UFUNCTION(BlueprintCallable, Category="Inventory|Slot")
	void UpdateSlot(FItemStructure ItemInfo);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Slot")
	int32 ItemIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Slot")
	E_ContainerType ContainerType; // Now recognized, because we've included the proper header.

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	
	// Called when the async asset load completes.
	UFUNCTION(BlueprintCallable, Category = "Inventory|Slot")
	void OnItemAssetLoaded();

	// Updates all UI elements based on the loaded item.
	UFUNCTION(BlueprintCallable, Category = "Inventory|Slot")
	void UpdateUIElements();

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
