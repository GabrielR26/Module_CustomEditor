
#include "CustomEditor.h"

#if !UE_BUILD_SHIPPING
#include "ToolMenus.h"
#include "GameProjectUtils.h"
#include "SourceCodeNavigation.h"
#include "Dialogs/SOutputLogDialog.h"
#include "FileHelpers.h"
#include "LevelEditorMenuContext.h"
#include "UnrealEdGlobals.h"
#include "GameFramework/GameUserSettings.h"
#include "Widgets/SViewport.h"
#endif

#define LOCTEXT_NAMESPACE "CustomEditorModule"

#if !UE_BUILD_SHIPPING
void FCustomEditorModule::StartupModule()
{
	/// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	ToolMenus = UToolMenus::Get();
	if (!ToolMenus)
	{
		UE_LOG(LogCustomEditor, Error, TEXT("Failed to get ToolMenus. Disable Tool Editor"));
		return;
	}

	CreateEditorMenu();
	//AddToolBarCommand();
}

void FCustomEditorModule::ShutdownModule()
{
	/// This function may be called during shutdown to clean up your module.
	/// For modules that support dynamic reloading, we call this function before unloading the module.
	if (ToolMenus)
		ToolMenus->UnregisterOwner(this);
}

void FCustomEditorModule::CreateEditorMenu()
{
	/// Get Editor's main menu to extend it
	UToolMenu* MainMenu = ToolMenus->ExtendMenu("LevelEditor.MainMenu");

	/// Get the current IDE's name if found, "IDE" by default
	FText ShortIDEName = FSourceCodeNavigation::GetSelectedSourceCodeIDE();
	if (ShortIDEName.ToString() == "None")
		ShortIDEName = FText::FromString("IDE");

	/// Add the SubMenu "Editor"
	UToolMenu* EditorMenu = MainMenu->AddSubMenu(
		MainMenu->MenuOwner,
		NAME_None,
		FName("Editor"),
		INVTEXT("Editor"),
		INVTEXT("Editor Options")
	);

	/// Add the section "EDITOR TOOL"
	FToolMenuSection& Section = EditorMenu->AddSection(
		"Editor tool",
		INVTEXT("EDITOR TOOL")
	);

	/// Add the command "Restart"
	Section.AddMenuEntry(
		"RestartCommand",
		INVTEXT("Restart"),
		INVTEXT("Restart the Editor"),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Refresh"),
		FUIAction(FExecuteAction::CreateStatic(&FCustomEditorModule::RestartProject))
	);

	/// Add the command "Rebuild"
	Section.AddMenuEntry(
		"RebuildCommand",
		INVTEXT("Rebuild"),
		FText::Format(INVTEXT("Close Editor and {0} Project, delete Intermediate and Binaries folder and relaunch uproject"), ShortIDEName),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Download"),
		FUIAction(FExecuteAction::CreateStatic(&FCustomEditorModule::RebuildProject))
	);

	/// Add the command "Open Project Solution"
	Section.AddDynamicEntry("CodeProject", FNewToolMenuSectionDelegate::CreateLambda([ShortIDEName](FToolMenuSection& InSection)
		{
			/// Update label, description and icon depending on project's solution 
			FText Label = FText();
			FText ToolTip = FText();
			FSlateIcon Icon = FSlateIcon();

			if (FSourceCodeNavigation::DoesModuleSolutionExist())
			{
				Label = FText::Format(INVTEXT("Refresh {0} Project"), ShortIDEName);
				ToolTip = FText::Format(INVTEXT("Refreshes your C++ code project in {0}."), ShortIDEName);
				Icon = FSourceCodeNavigation::GetRefreshSourceCodeIDEIcon();
			}
			else
			{
				Label = FText::Format(INVTEXT("Generate {0} Project"), ShortIDEName);
				ToolTip = FText::Format(INVTEXT("Generates your C++ code project in {0}."), ShortIDEName);
				Icon = FSourceCodeNavigation::GetOpenSourceCodeIDEIcon();
			}

			InSection.AddMenuEntry(
				"OpenSolutionCommand",
				Label,
				ToolTip,
				Icon,
				FUIAction(FExecuteAction::CreateStatic(&FCustomEditorModule::OpenSolution))
			);
		}));

	/// Add the command "Documentation"
	Section.AddMenuEntry(
		"OpenDocumentationCommand",
		INVTEXT("Documentation"),
		INVTEXT("Open tool documentation"),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Help"),
		FUIAction(FExecuteAction::CreateStatic(&FCustomEditorModule::OpenDocumentation))
	);
}

void FCustomEditorModule::AddToolBarCommands()
{
	/// Add button on ToolBar 
	UToolMenu* ToolbarMenu = ToolMenus->ExtendMenu("LevelEditor.LevelEditorToolBar.User");
	FToolMenuSection& ToolbarSection = ToolbarMenu->FindOrAddSection("Custom");

	ToolbarSection.AddEntry(
		FToolMenuEntry::InitToolBarButton(
			FName("Tool"),
			FExecuteAction::CreateStatic(&FCustomEditorModule::ToolAction),
			INVTEXT("Tool"),
			INVTEXT("The tool description"),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Warning"))
	);
}

void FCustomEditorModule::RestartProject()
{
	// Check Editor can restart
	if (!GIsEditor)
	{
		UE_LOG(LogCustomEditor, Error, TEXT("Restart only available from Editor"));
		return;
	}

	// Save all
	const bool bPromptUserToSave = true;
	const bool bSaveMapPackages = true;
	const bool bSaveContentPackages = true;
	const bool bFastSave = false;
	const bool bNotifyNoPackagesSaved = false;
	const bool bCanBeDeclined = false;
	FEditorFileUtils::SaveDirtyPackages(bPromptUserToSave, bSaveMapPackages, bSaveContentPackages, bFastSave, bNotifyNoPackagesSaved, bCanBeDeclined);

	// Restart Editor
	FUnrealEdMisc::Get().RestartEditor(false);

	UE_LOG(LogCustomEditor, Display, TEXT("Restart project"));
}

void FCustomEditorModule::RebuildProject()
{
	// Check Editor can restart
	if (!GIsEditor)
	{
		UE_LOG(LogCustomEditor, Error, TEXT("Restart only available from Editor"));
		return;
	}

	// Save all
	const bool bPromptUserToSave = false;
	const bool bSaveMapPackages = true;
	const bool bSaveContentPackages = true;
	const bool bFastSave = false;
	const bool bNotifyNoPackagesSaved = false;
	const bool bCanBeDeclined = false;
	FEditorFileUtils::SaveDirtyPackages(bPromptUserToSave, bSaveMapPackages, bSaveContentPackages, bFastSave, bNotifyNoPackagesSaved, bCanBeDeclined);

	// To close Editor
	FPlatformMisc::RequestExit(false);

	// Get python executable path
	FString _pathPythonExe = FPaths::RootDir() + PATH_DIRECTORY_PYTHON_EXEC;
	UE_LOG(LogCustomEditor, Display, TEXT("python: %s"), *_pathPythonExe);
	if (!FPaths::FileExists(_pathPythonExe))
	{
		UE_LOG(LogCustomEditor, Error, TEXT("Failed to find python exec. Abort rebuild"));
		return;
	}
	// Get python script path
	FString _pathPythonScript = FPaths::ProjectPluginsDir() + PATH_SCRIPT_PYTHON_REBUILD;
	UE_LOG(LogCustomEditor, Display, TEXT("script: %s"), *_pathPythonScript);
	if (!FPaths::FileExists(_pathPythonScript))
	{
		UE_LOG(LogCustomEditor, Error, TEXT("Failed to find python script. Abort rebuild"));
		return;
	}

	// Launch python script
	FPlatformProcess::CreateProc(
		*_pathPythonExe,
		*_pathPythonScript,
		true, false, false, nullptr, 0, nullptr, nullptr
	);

	UE_LOG(LogCustomEditor, Display, TEXT("Rebuild project"));
}

void FCustomEditorModule::OpenSolution()
{
	// Generate/Refresh code project
	if (!FSourceCodeNavigation::IsCompilerAvailable())
	{
		// Attempt to trigger the tutorial if the user doesn't have a compiler installed for the project.
		FSourceCodeNavigation::AccessOnCompilerNotFound().Broadcast();
	}
	FText FailReason, FailLog;
	if (!FGameProjectGenerationModule::Get().UpdateCodeProject(FailReason, FailLog))
		SOutputLogDialog::Open(LOCTEXT("RefreshProject", "Refresh Project"), FailReason, FailLog);

	// Can Open IDE
	const bool bIsCodeProject = IFileManager::Get().DirectoryExists(*FPaths::GameSourceDir());
	if (ensure(GUnrealEd) && bIsCodeProject)
	{
		// Open IDE
		if (!FSourceCodeNavigation::IsCompilerAvailable())
		{
			// Attempt to trigger the tutorial if the user doesn't have a compiler installed for the project.
			FSourceCodeNavigation::AccessOnCompilerNotFound().Broadcast();
		}
		else
		{
			if (!FSourceCodeNavigation::OpenModuleSolution())
			{
				FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("OpenIDEFailed_UnableToOpenSolution", "Unable to open solution"));
			}
		}
	}

	UE_LOG(LogCustomEditor, Display, TEXT("Refresh code project"));
}

void FCustomEditorModule::OpenDocumentation()
{
	FString _url = FString(URL_DOCUMENTATION);
	FPlatformProcess::LaunchURL(*_url, NULL, NULL);
}

void FCustomEditorModule::ToolAction()
{
	UE_LOG(LogCustomEditor, Display, TEXT("The tool action"));
}

#endif

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FCustomEditorModule, CustomEditor)