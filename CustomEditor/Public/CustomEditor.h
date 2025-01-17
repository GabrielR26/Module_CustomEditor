#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCustomEditor, All, All) /// Logging category for all files including .h
DEFINE_LOG_CATEGORY(LogCustomEditor)

#define PATH_DIRECTORY_PYTHON_EXEC "Engine/Binaries/ThirdParty/Python3/Win64/python.exe"
#define PATH_SCRIPT_PYTHON_REBUILD "Python/rebuild_project.py"
#define URL_DOCUMENTATION "https://github.com/GabrielR26/Module_CustomEditor"

class FCustomEditorModule : public IModuleInterface
{
#if !UE_BUILD_SHIPPING /// Use preprocess to allow shipping
    UToolMenus* ToolMenus = nullptr;

public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    /// <summary>
    /// Add the "Editor" menu on Editor's MainMenu 
    /// </summary>
    void CreateEditorMenu();
    /// <summary>
    /// Add tool commands on Editor's ToolBar 
    /// </summary>
    void AddToolBarCommands();

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
    /// <summary>
    /// Use Unreal's methods body to resize viewport
    /// </summary>
    static void ToolAction();
#endif
};
