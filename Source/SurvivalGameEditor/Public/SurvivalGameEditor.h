#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "UObject/StrongObjectPtr.h"
#include "Tools/ItemFactory.h"

/**
 * Editor module for the SurvivalGame. Holds an ItemFactory instance for editor-only usage.
 */
class FSurvivalGameEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    /** Returns the ItemFactory instance (if needed elsewhere). */
    UItemFactory* GetItemFactory() const { return ItemFactory.Get(); }

private:
    /** Kept alive with a TStrongObjectPtr so it doesn't get GC'ed until we Reset() it. */
    TStrongObjectPtr<UItemFactory> ItemFactory;
};
