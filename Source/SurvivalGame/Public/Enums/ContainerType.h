// ContainerType.h

#pragma once

#include "CoreMinimal.h"
#include "ContainerType.generated.h"

/**
 * @brief Enum representing different types of item containers within the game.
 * 
 * This enum is used to categorize containers such as Inventory, Hotbar, Storage, etc.
 * It facilitates organized inventory management, UI representation, and gameplay mechanics.
 */
UENUM(BlueprintType)
enum class E_ContainerType : uint8
{
    /** Represents no container type. Useful as a default or uninitialized state. */
    None            UMETA(DisplayName = "None"),

    /** Primary storage for player items. Typically accessible via inventory UI. */
    Inventory       UMETA(DisplayName = "Inventory"),

    /** Quick-access bar for frequently used items or equipment. */
    Hotbar          UMETA(DisplayName = "Hotbar"),

    /** General storage containers like chests, barrels, or backpacks. */
    Storage         UMETA(DisplayName = "Storage"),

    /** Specialized containers for armor or wearable items. */
    Armor           UMETA(DisplayName = "Armor"),

    /** Containers managed by AI companions, allowing them to carry items. */
    AICompanion     UMETA(DisplayName = "AI Companion"),

    /** Temporary containers used during crafting or item transactions. */
    Temporary       UMETA(DisplayName = "Temporary"),

    /** Containers specific to certain game modes or features (e.g., quest items). */
    Special         UMETA(DisplayName = "Special"),

    /** Hidden containers not visible to the player but used for backend processes. */
    Hidden          UMETA(DisplayName = "Hidden")
};

/**
 * @brief Helper namespace for E_ContainerType related utility functions.
 */
namespace ContainerTypeHelpers
{
    /**
     * @brief Checks if the given container type is valid (i.e., not None).
     * 
     * @param ContainerType The container type to validate.
     * @return true If the container type is valid.
     * @return false If the container type is None.
     */
    FORCEINLINE bool IsValidContainerType(E_ContainerType ContainerType)
    {
        return ContainerType != E_ContainerType::None;
    }

    /**
     * @brief Retrieves a string representation of the container type.
     * 
     * Useful for debugging, logging, or displaying container types in the UI.
     * 
     * @param ContainerType The container type to convert to string.
     * @return FString The string representation of the container type.
     */
    FORCEINLINE FString GetContainerTypeAsString(E_ContainerType ContainerType)
    {
        switch (ContainerType)
        {
            case E_ContainerType::None:
                return TEXT("None");
            case E_ContainerType::Inventory:
                return TEXT("Inventory");
            case E_ContainerType::Hotbar:
                return TEXT("Hotbar");
            case E_ContainerType::Storage:
                return TEXT("Storage");
            case E_ContainerType::Armor:
                return TEXT("Armor");
            case E_ContainerType::AICompanion:
                return TEXT("AI Companion");
            case E_ContainerType::Temporary:
                return TEXT("Temporary");
            case E_ContainerType::Special:
                return TEXT("Special");
            case E_ContainerType::Hidden:
                return TEXT("Hidden");
            default:
                return TEXT("Unknown");
        }
    }

    /**
     * @brief Converts a string to its corresponding E_ContainerType enum value.
     * 
     * Useful for parsing configuration files, user input, or network data.
     * 
     * @param ContainerTypeString The string representation of the container type.
     * @return E_ContainerType The corresponding enum value. Returns E_ContainerType::None if not found.
     */
    FORCEINLINE E_ContainerType GetContainerTypeFromString(const FString& ContainerTypeString)
    {
        if (ContainerTypeString.Equals(TEXT("Inventory"), ESearchCase::IgnoreCase))
        {
            return E_ContainerType::Inventory;
        }
        else if (ContainerTypeString.Equals(TEXT("Hotbar"), ESearchCase::IgnoreCase))
        {
            return E_ContainerType::Hotbar;
        }
        else if (ContainerTypeString.Equals(TEXT("Storage"), ESearchCase::IgnoreCase))
        {
            return E_ContainerType::Storage;
        }
        else if (ContainerTypeString.Equals(TEXT("Armor"), ESearchCase::IgnoreCase))
        {
            return E_ContainerType::Armor;
        }
        else if (ContainerTypeString.Equals(TEXT("AI Companion"), ESearchCase::IgnoreCase))
        {
            return E_ContainerType::AICompanion;
        }
        else if (ContainerTypeString.Equals(TEXT("Temporary"), ESearchCase::IgnoreCase))
        {
            return E_ContainerType::Temporary;
        }
        else if (ContainerTypeString.Equals(TEXT("Special"), ESearchCase::IgnoreCase))
        {
            return E_ContainerType::Special;
        }
        else if (ContainerTypeString.Equals(TEXT("Hidden"), ESearchCase::IgnoreCase))
        {
            return E_ContainerType::Hidden;
        }
        else
        {
            return E_ContainerType::None;
        }
    }
}