// InventoryWidget.cpp
#include "UI/Widgets/InventoryWidget.h"

UInventoryWidget::UInventoryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
    
	if (ValidateWidgetReferences())
	{
		InitializeWidget();
	}
}

void UInventoryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UInventoryWidget::InitializeWidget()
{
	// Initialize the container grid if needed
	if (ItemContainerGrid)
	{
		// Additional initialization can be added here
		UE_LOG(LogTemp, Log, TEXT("ItemContainerGrid initialized in InventoryWidget"));
	}
}

bool UInventoryWidget::ValidateWidgetReferences() const
{
	bool bIsValid = true;

	if (!ItemContainerGrid)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: ItemContainerGrid not found! Make sure it's bound in the Blueprint."), *GetName());
		bIsValid = false;
	}

	return bIsValid;
}