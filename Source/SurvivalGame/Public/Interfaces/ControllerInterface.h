#pragma once

#include "UObject/Interface.h"
#include "Enums/ContainerType.h"
#include "Data/Struct/ItemStructure.h"
#include "ControllerInterface.generated.h"

/**
 * @brief Interface for controller functions, such as closing the inventory UI.
 */
UINTERFACE(BlueprintType)
class SURVIVALGAME_API UControllerInterface : public UInterface
{
	GENERATED_BODY()
};

class SURVIVALGAME_API IControllerInterface
{
	GENERATED_BODY()

public:
	// BlueprintNativeEvent to allow a C++ default implementation or a Blueprint override.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
	void CloseInventory();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
	void UpdateItemSlot(E_ContainerType ContainerType, FItemStructure ItemInfo, int32 Index);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
	void ResetItemSlot(E_ContainerType ContainerType, int32 Index);
	
};
