
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
#include "EditorUtilityWidgetBlueprint.h"
#include "EditorUtilitySubsystem.h"
#include "Editor/Blutility/Classes/EditorUtilityWidget.h"
#include "./../Plugins/Editor/EditorScriptingUtilities/Source/EditorScriptingUtilities/Public/EditorAssetLibrary.h"
#include "Styling/CoreStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "UObject/UObjectGlobals.h"
#include "CustomEditorSettings.h"
#endif

#define LOCTEXT_NAMESPACE "CustomEditorModule" 

#if !UE_BUILD_SHIPPING
TSharedPtr<FSlateStyleSet> FCustomEditorModule::StyleSet = NULL;

void FCustomEditorModule::StartupModule()
{
	/// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	ToolMenus = UToolMenus::Get();
	if (!ToolMenus)
	{
		UE_LOG(LogCustomEditor, Error, TEXT("Failed to get ToolMenus. Disable Tool Editor"));
		return;
	}

	UCustomEditorSettings* Settings = GetMutableDefault<UCustomEditorSettings>();
	Settings->InitSettings();
	
	CreateEditorMenu();
	CreateCustomMenu();
	AddToolBarCommands();
}

void FCustomEditorModule::ShutdownModule()
{
	/// This function may be called during shutdown to clean up your module.
	/// For modules that support dynamic reloading, we call this function before unloading the module.
	if (ToolMenus)
		ToolMenus->UnregisterOwner(this);
}

#pragma region MainMenu
void FCustomEditorModule::CreateEditorMenu()
{
	UToolMenu* MainMenu = ToolMenus->ExtendMenu("LevelEditor.MainMenu");
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
	//UE_LOG(LogCustomEditor, Warning, TEXT("python: %s"), *_pathPythonExe);
	if (!FPaths::FileExists(_pathPythonExe))
	{
		UE_LOG(LogCustomEditor, Error, TEXT("Failed to find python exec. Abort rebuild"));
		return;
	}
	// Get python script path
	FString _pathPythonScript = FPaths::ProjectDir() + PATH_SCRIPT_PYTHON_REBUILD;
	//UE_LOG(LogCustomEditor, Warning, TEXT("script: %s"), *_pathPythonScript);
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
	// Create/Refresh code project
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

	UE_LOG(LogCustomEditor, Display, TEXT("Open documentation module"));
}
#pragma endregion

#pragma region Custom
void FCustomEditorModule::CreateCustomMenu()
{
	UToolMenu* MainMenu = ToolMenus->ExtendMenu("LevelEditor.MainMenu");

	/// Add the SubMenu "Custom"
	UToolMenu* EditorMenu = MainMenu->AddSubMenu(
		MainMenu->MenuOwner,
		NAME_None,
		FName("Custom"),
		INVTEXT("Custom"),
		INVTEXT("Custom Options")
	);

	/// Add the section "COMABT TOOL"
	FToolMenuSection& Section = EditorMenu->AddSection(
		"Custom tool",
		INVTEXT("CUSTOM TOOL")
	);

	/// Add the command "Toggle"
	Section.AddMenuEntry(
		"ToggleCommand",
		INVTEXT("Toggle"),
		INVTEXT("Toggle tool"),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateStatic(&FCustomEditorModule::SetToggle),
			FCanExecuteAction::CreateStatic(&FCustomEditorModule::CanExecute),
			FIsActionChecked::CreateStatic(&FCustomEditorModule::SetToggleIsChecked)
			),
		EUserInterfaceActionType::ToggleButton
	);
}

bool FCustomEditorModule::CanExecute()
{
	return true;
}

void FCustomEditorModule::SetToggle()
{
	UCustomEditorSettings* Settings = GetMutableDefault<UCustomEditorSettings>();
	Settings->Toggle();

	UE_LOG(LogCustomEditor, Display, TEXT("Toggle command"));
}

bool FCustomEditorModule::SetToggleIsChecked()
{
	return GetMutableDefault<UCustomEditorSettings>()->GetToggle();
}
#pragma endregion

void FCustomEditorModule::AddToolBarCommands()
{
	// Add button on ToolBar 
	UToolMenu* ToolbarMenu = ToolMenus->ExtendMenu("LevelEditor.LevelEditorToolBar.User");
	FToolMenuSection& ToolbarSection = ToolbarMenu->FindOrAddSection("Custom");

	// Register custom icon
	StyleSet = MakeShareable(new FSlateStyleSet("CustomStyle"));
	// Set paths to content folders to support loading assets that ship with the engine
	StyleSet->SetCoreContentRoot(FPaths::EngineContentDir() / TEXT("Slate"));
	// Register some brushes
	const FVector2D Icon32x32(32.0f, 32.0f);
	FString _path = FPaths::ProjectDir() / TEXT("Source/CustomEditor/Content/IconGithub.png");
	StyleSet->Set("Custom.IconGithub", new FSlateImageBrush(_path, Icon32x32));
	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());

	// Add Icon menu on toolbar
	FToolMenuEntry IconMenu = FToolMenuEntry::InitComboButton(
		FName("Icon"),
		FToolUIActionChoice(),
		FOnGetContent::CreateRaw(this, &FCustomEditorModule::GenerateEUWMenuWidget),
		INVTEXT("Icon"),
		INVTEXT("Icon options"),
		FSlateIcon(FName("CustomStyle"), "Custom.IconGithub")
	);
	IconMenu.StyleNameOverride = "CalloutToolbar";
	ToolbarSection.AddEntry(IconMenu);
}

#pragma region EUW
TSharedRef<SWidget> FCustomEditorModule::GenerateEUWMenuWidget()
{
	if (ToolMenus->IsMenuRegistered("LevelEditor.LevelEditorToolBar.TechMenu"))
		return ToolMenus->GenerateWidget("LevelEditor.LevelEditorToolBar.TechMenu", nullptr);

	UToolMenu* TechMenu = ToolMenus->RegisterMenu("LevelEditor.LevelEditorToolBar.TechMenu");
	{
		// Add section "EditorUtilityWidget"
		FToolMenuSection& EUWSection = TechMenu->AddSection("EUW", INVTEXT("EUW"));
		// Add the command "EUW"
		EUWSection.AddMenuEntry(
			"EUWCommand",
			INVTEXT("EUW"),
			INVTEXT("EUW Tool"),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateStatic(&FCustomEditorModule::RunEUW, FString("Full reference to EUW")))
		);
	}

	return ToolMenus->GenerateWidget(TechMenu);
}

void FCustomEditorModule::RunEUW(FString _EUWReference)
{
	UBlueprint* UMGBP = LoadObject<UBlueprint>(nullptr, *_EUWReference);
	UEditorUtilityWidgetBlueprint* EditorWidget = Cast<UEditorUtilityWidgetBlueprint>(UMGBP);
	if (EditorWidget)
	{
		UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();
		EditorUtilitySubsystem->SpawnAndRegisterTab(EditorWidget);
	}

	UE_LOG(LogCustomEditor, Display, TEXT("Run EUW"));
}
#pragma endregion
#endif

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FCustomEditorModule, CustomEditor)