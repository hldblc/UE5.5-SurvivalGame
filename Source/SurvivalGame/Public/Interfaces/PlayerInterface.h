#pragma once

#include "UObject/Interface.h"
#include "Enums/ContainerType.h"
#include "Data/Struct/ItemStructure.h"
#include "PlayerInterface.generated.h"


UINTERFACE(BlueprintType)
class SURVIVALGAME_API UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

class SURVIVALGAME_API IPlayerInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
	void UpdateItem(E_ContainerType ContainerType, FItemStructure ItemInfo, int32 Index);
	

	
};
