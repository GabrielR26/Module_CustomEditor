#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h" 

DECLARE_LOG_CATEGORY_EXTERN(LogCustomEditor, All, All) /// Logging category for all files including .h
DEFINE_LOG_CATEGORY(LogCustomEditor)

#define PATH_DIRECTORY_PYTHON_EXEC "Engine/Binaries/ThirdParty/Python3/Win64/python.exe"
#define PATH_SCRIPT_PYTHON_REBUILD "Source/CustomEditor/Python/rebuild_project.py"
#define URL_DOCUMENTATION "https://github.com/GabrielR26/Module_CustomEditor/blob/main/README.md"

class FCustomEditorModule : public IModuleInterface
{
#if !UE_BUILD_SHIPPING /// Use preprocess to allow shipping
    UToolMenus* ToolMenus = nullptr;
    static TSharedPtr<class FSlateStyleSet > StyleSet;

public:
    void StartupModule() override;
    void ShutdownModule() override;

private:
#pragma region Editor
    /// <summary>
    /// Add the "Editor" menu on Editor's MainMenu 
    /// </summary>
    void CreateEditorMenu();
    /// <summary>
    /// Use Unreal's methods body to restart Editor
    /// </summary>
    static void RestartProject();
    /// <summary>
    /// Use Unreal's methods body and python script to rebuild project
    /// </summary>
    static void RebuildProject();
    /// <summary>
    /// Use Unreal's methods body to generate/refresh and open project's solution with actual IDE
    /// </summary>
    static void OpenSolution();
    /// <summary>
    /// Use Unreal's methods body to open the tool's documentation web page 
    /// </summary>
    static void OpenDocumentation();
#pragma endregion

#pragma region Combat
    /// <summary>
    /// Add the "Combat" menu on Editor's MainMenu 
    /// </summary>
    void CreateCustomMenu();
    static bool CanExecute();

    static void SetToggle();
    static bool SetToggleIsChecked();
#pragma endregion

#pragma region Tech
    /// <summary>
    /// Add tool commands on Editor's ToolBar 
    /// </summary>
    void AddToolBarCommands();

    /// <summary>
    /// Add tech submenu on Editor's ToolBar
    /// </summary>
    /// <returns> The widget to display </returns>
    TSharedRef<SWidget> GenerateEUWMenuWidget();
    /// <summary>
    /// Run an EditorUtilityWidget
    /// </summary>
    static void RunEUW(FString _EUWReference);
#pragma endregion
#endif
};
