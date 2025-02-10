# Module_CustomEditor
 An Unreal Engine module to add new menu and command on Editor's menu and toolbar

# SETUP
- A) Get the module, unzip and place it in your Root/Source project folder, next to your project's module (with the name of your project)

![image](https://github.com/user-attachments/assets/efa7b82f-f1c0-4605-ae97-d6b88751898f)

- B) Open your project.uproject (in an IDE)

![image](https://github.com/user-attachments/assets/7eb6478b-4571-4fea-a4e4-7ac4bb74daa1)

and add the CustomEditor module in the "Modules" section with the Type = Editor : 

```cpp
{
   "Name": "CustomEditor",
   "Type": "Editor",
   "LoadingPhase": "Default"
}
```

- C) Open your projectEditor.Target.cs

![image](https://github.com/user-attachments/assets/4e38cb8b-1368-4f3b-a50a-36513ac9d30b)

and replace the line 
```cpp
ExtraModuleNames.Add("YourProject");
```
by 
```cpp
ExtraModuleNames.AddRange(new string[] { "UnrealProject", "CustomEditor" });
```

- D) Build the project's solution and launch your project. The module start now when the editor is update (= open) and add all menus and commands added. 

# TOOL EDITOR
The module is include with a new menu in the Unreal Editor's main menu, call "Editor" with few commands to help on developping Unreal project

![image](https://github.com/user-attachments/assets/27bb34f9-07e5-4fc0-ba8c-8140e097df07)

- Restart => Close and open the Editor (usefull when adding new menus and commands and see what you did fastly).
- Rebuild => Close the Editor and your current IDE project's solution if open (WARNING : save your files before), delete the project's solution and Binaries and Intermediates folder, and launch the .uproject (that display the generation's choice pop-up of Unreal Engine).
- Generate/Refresh IDE project => Generate IDE project's solution if doesn't exist or refresh it, then open in your current IDE (if not open yet).
- Documentation => Open the documentation tool (here).

# TUTORIAL
In order to add some menus and commands in the Unreal Engine Editor's menus and toolbars, you need to know the name of it :
- A) In the Unreal's console command, enter this line "ToolMenus.Edit 1" => This will activate the ToolMenus edit fonctionality in Editor.

![image](https://github.com/user-attachments/assets/f8e4d34f-89ec-48f5-93ef-6b91152a0664)

- B) Now you can see some gear button and an "Edit menu" menu in Unreal Editor's menus and toolbars. When you click on it, this will display a new window that display information and the path name of the current location.

![ezgif com-video-to-gif-converter](https://github.com/user-attachments/assets/939d8798-cb03-4df6-a703-573aef1178b1)

- C) Use the path name to extend the menu (toolbar == menu) and add command or submenu in it
```cpp
UToolMenu* MainMenu = ToolMenus->ExtendMenu("LevelEditor.MainMenu");
```

- D) To use custom icon, you need to create your own style and apply it on your command. See exemple in the method on CustomEdditor.cpp : 
```cpp
void AddToolBarCommands();
```
- E) To persistently save your data (bool, int, ...), save it in persistent file (.json, ...) and save/load it has your need. See some exemple in class CustomEditorSettings.

- F) To launch 'EditorUtilityWidget' from your custom command, use the EditorAssetLibrary. See exemple in the method on CustomEdditor.cpp : 
```cpp
static void RunEUW(FString _EUWReference);
```

# NOTE
- If you want to use python script, store it in "Python" folder and use the same way in "RebuildProject" method to use it.
- To see your added menus/commands in Unreal Editor, you need to refresh it, like using the restart command.
- The big part of the code was find in Unreal Engine source code, so owning by Epic Games Inc.
- Made on Unreal Engine 5.4 release. Don't know if work on previous release.
