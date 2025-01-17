import os
import subprocess

# Execute python script separately to free Unreal Engine process (to close it)

script_path = os.path.dirname(os.path.abspath(__file__))
script_path = os.path.join(script_path, "rebuild_project_process.py")
subprocess.Popen(["python", script_path])
