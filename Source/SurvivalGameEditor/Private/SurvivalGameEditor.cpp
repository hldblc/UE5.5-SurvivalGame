#include "SurvivalGameEditor.h"
#include "Tools/ItemFactory.h"
#include "UObject/StrongObjectPtr.h"

#define LOCTEXT_NAMESPACE "FSurvivalGameEditorModule"

void FSurvivalGameEditorModule::StartupModule()
{
	// Create the ItemFactory object in the transient package
	UItemFactory* FactoryObject = NewObject<UItemFactory>(GetTransientPackage(), UItemFactory::StaticClass());

	// Assign to TStrongObjectPtr, which keeps a valid reference to the object
	ItemFactory = TStrongObjectPtr<UItemFactory>(FactoryObject);
}

void FSurvivalGameEditorModule::ShutdownModule()
{
	// TStrongObjectPtr automatically unroots the object if it’s still valid
	ItemFactory.Reset();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FSurvivalGameEditorModule, SurvivalGameEditor)
