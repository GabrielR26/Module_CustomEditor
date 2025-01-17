# Module_CustomEditor
 An Unreal Engine module to add new menu and command on Editor's menu and toolbar

# SETUP
Get the module, unzip and place it in your Root/Source project folder, next to your project's module (with the name of your project)

![image](https://github.com/user-attachments/assets/efa7b82f-f1c0-4605-ae97-d6b88751898f)

# UTILIZATION
This add a new menu in the Editor main menu, call "Editor" :

![image](https://github.com/user-attachments/assets/e5cd3da7-1bab-4412-8c79-e35a4dd03c75)

- Restart => Close and open the Editor.
- Rebuild => Close the Editor and your current IDE project's solution if open (WARNING : save your files before), delete the project's solution and Binaries and Intermediates folder, and launch the .uproject (that display the generation's choice pop-up of Unreal Engine).
- Generate/Refresh IDE project => Generate IDE project's solution if doesn't exist or refresh it, then open in your current IDE (if not open yet).
- Documentation => Open the documentation tool (here).

# TUTORIAL
In order to add some commands/menus in the Unreal Engine Editor's menus and toolbars, you need to know the name of it :
- In the Unreal's console command, enter this line "ToolMenus.Edit 1" => This will activate the ToolMenus edit fonctionality in Editor.
![image](https://github.com/user-attachments/assets/f8e4d34f-89ec-48f5-93ef-6b91152a0664)
