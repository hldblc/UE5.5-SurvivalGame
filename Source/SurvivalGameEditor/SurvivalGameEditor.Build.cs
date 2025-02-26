using UnrealBuildTool;

public class SurvivalGameEditor : ModuleRules
{
	public SurvivalGameEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"SurvivalGame",
			"UMG",            // Add this
			"Slate",
			"SlateCore",
			"Blutility",      // Add this
			"UMGEditor"       // Add this
		});

		PrivateDependencyModuleNames.AddRange(new string[] {
			"UnrealEd",
			"EditorFramework",
			"EditorStyle",
			"EditorSubsystem",
			"AssetTools",
			"ContentBrowser",
			"EditorScriptingUtilities",
			"DeveloperSettings",
			"Projects",        // Add this
			"InputCore",       // Add this
			"ToolMenus",       // Add this
			"ApplicationCore"  // Add this
		});
	}
}