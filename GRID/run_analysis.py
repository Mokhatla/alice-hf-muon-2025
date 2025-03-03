import os
import subprocess
import shutil

# Define paths
original_file = os.path.expanduser("~/Documents/alice/GRID/MC/2024/LHC18h2_extra/runAnalysis.C")
backup_file = os.path.expanduser("~/Documents/alice/GRID/MC/2024/LHC18h2_extra/runAnalysis.C.bak")
copy_file = os.path.expanduser("~/Documents/alice/GRID/MC/2024/LHC18h2_extra/runAnalysis_full.C")

def run_original_file():
    # Run the original runAnalysis.C file
    subprocess.run(["aliroot", original_file], check=True, shell=True, executable="/bin/zsh")

def make_copy_and_rename():
    # Make a copy and rename it
    shutil.copy2(original_file, copy_file)
    print(f"Copied {original_file} to {copy_file}")

def edit_original_file():
    # Backup the original file
    shutil.copy2(original_file, backup_file)
    print(f"Backup of original file created at {backup_file}")
    
    # Read the original file
    with open(original_file, 'r') as file:
        filedata = file.read()
    
    # Replace the target string
    filedata = filedata.replace('alienHandler->SetRunMode("full");', 'alienHandler->SetRunMode("terminate");')
    
    # Write the modified file
    with open(original_file, 'w') as file:
        file.write(filedata)
    print(f"Edited {original_file} to change run mode to terminate")

def main():
    print("Running the original runAnalysis.C file...")
    run_original_file()
    
    print("Making a copy and renaming it to runAnalysis_full.C...")
    make_copy_and_rename()
    
    print("Editing the original runAnalysis.C file...")
    edit_original_file()

if __name__ == "__main__":
    main()