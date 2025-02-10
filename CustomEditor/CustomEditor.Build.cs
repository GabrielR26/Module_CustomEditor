using UnrealBuildTool;

public class CustomEditor : ModuleRules
{
    public CustomEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "Json"
            }            
        );
        
        if (Target.Type == TargetRules.TargetType.Editor)
        {
            PrivateDependencyModuleNames.AddRange(new string[]
            {
                "EditorFramework",
                "UnrealEd",
                "GameProjectGeneration",
                "MainFrame",
                "LevelEditor",
                "ToolMenus",
                "Blutility",
                "UMGEditor",
                "EditorScriptingUtilities",
            });
        }
    }
}