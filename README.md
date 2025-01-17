# Module_CustomEditor
 An Unreal Engine module to add new menu and command on Editor's menu and toolbar

# SETUP
- A) Get the module, unzip and place it in your Root/Source project folder, next to your project's module (with the name of your project)

![image](https://github.com/user-attachments/assets/efa7b82f-f1c0-4605-ae97-d6b88751898f)

- B) Open your project.uproject (in an IDE)

![image](https://github.com/user-attachments/assets/7eb6478b-4571-4fea-a4e4-7ac4bb74daa1)

and add the CustomEditor module in the "Modules" section with the Type = Editor : 

![image](https://github.com/user-attachments/assets/483d9127-3fd2-4ecf-9f7e-a0d3ea183894)

- C) Open your project.Target.cs and projectEditor.Target.cs

![image](https://github.com/user-attachments/assets/4e38cb8b-1368-4f3b-a50a-36513ac9d30b)

and replace the line 'ExtraModuleNames.Add("YourProject");' by 'ExtraModuleNames.AddRange(new string[] { "UnrealProject", "CustomEditor" });' on both files

![image](https://github.com/user-attachments/assets/9755a4d1-ac61-46d0-96f1-c702988a5b72)

- D) Build the project's solution and launch your project. The module start now when the editor is update (= open) and add all menus and commands added. 

# TOOL EDITOR
The module is include with a new menu in the Unreal Editor's main menu, call "Editor" with few commands to help on developping Unreal project

![image](https://github.com/user-attachments/assets/e5cd3da7-1bab-4412-8c79-e35a4dd03c75)

- Restart => Close and open the Editor (usefull when adding new menus and commands and watch what you did fastly).
- Rebuild => Close the Editor and your current IDE project's solution if open (WARNING : save your files before), delete the project's solution and Binaries and Intermediates folder, and launch the .uproject (that display the generation's choice pop-up of Unreal Engine).
- Generate/Refresh IDE project => Generate IDE project's solution if doesn't exist or refresh it, then open in your current IDE (if not open yet).
- Documentation => Open the documentation tool (here).

# TUTORIAL
In order to add some menus and commands in the Unreal Engine Editor's menus and toolbars, you need to know the name of it :
- A) In the Unreal's console command, enter this line "ToolMenus.Edit 1" => This will activate the ToolMenus edit fonctionality in Editor.

![image](https://github.com/user-attachments/assets/f8e4d34f-89ec-48f5-93ef-6b91152a0664)

- B) 
