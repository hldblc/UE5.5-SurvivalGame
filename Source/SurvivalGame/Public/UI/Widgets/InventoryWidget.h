// InventoryWidget.h
#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "UI/Widgets/Inventory/ItemContainerGrid.h"
#include "InventoryWidget.generated.h"

/**
 * @brief Widget class for managing the main inventory interface
 */
UCLASS(BlueprintType, Blueprintable, meta = (DisplayName = "Inventory Widget"))
class SURVIVALGAME_API UInventoryWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UInventoryWidget(const FObjectInitializer& ObjectInitializer);
    
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;

	/** Gets the item container grid widget */
	UFUNCTION(BlueprintCallable, Category = "UI")
	UItemContainerGrid* GetItemContainerGrid() const { return ItemContainerGrid; }

protected:
	/** The grid container for inventory slots. Bind this in Blueprint. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UItemContainerGrid> ItemContainerGrid;

	/** Initializes widget components */
	void InitializeWidget();

	/** Validates required widget references */
	bool ValidateWidgetReferences() const;
};