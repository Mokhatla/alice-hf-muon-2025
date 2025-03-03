import os
import subprocess
import sys

# Define the content of the setup script
setup_script_content = """
#!/bin/zsh

# Check if we are already in an AliPhysics environment
if alienv list | grep -q "AliPhysics/latest"; then
    echo "AliPhysics environment is already active."
else
    # Set up the AliPhysics environment
    alienv enter AliPhysics/latest
fi

# Execute the provided command
exec "$@"
"""

def write_setup_script():
    setup_script_path = "/tmp/setup_environment.sh"
    with open(setup_script_path, "w") as file:
        file.write(setup_script_content)
    os.chmod(setup_script_path, 0o755)
    return setup_script_path

def is_environment_active():
    try:
        result = subprocess.run(["alienv", "list"], capture_output=True, text=True, shell=True, executable="/bin/zsh")
        return "AliPhysics/latest" in result.stdout
    except subprocess.CalledProcessError:
        return False

def run_script_in_environment(script_path):
    setup_script_path = write_setup_script()
    result = subprocess.run(["zsh", setup_script_path, "python", script_path], executable="/bin/zsh")
    if result.returncode != 0:
        print(f"Error running the script: {script_path}")
        sys.exit(1)

def main():
    if len(sys.argv) < 2:
        print("Usage: python run_in_environment.py <python_script>")
        sys.exit(1)

    script_path = os.path.abspath(sys.argv[1])

    if not os.path.isfile(script_path):
        print(f"Error: Script file not found: {script_path}")
        sys.exit(1)

    if not is_environment_active():
        print("AliPhysics environment is not active. Setting up environment...")
        run_script_in_environment(script_path)
    else:
        print("AliPhysics environment is already active.")
        run_script(script_path)

def run_script(script_path):
    result = subprocess.run(["python", script_path], executable="/bin/zsh")
    if result.returncode != 0:
        print(f"Error running the script: {script_path}")
        sys.exit(1)

if __name__ == "__main__":
    main()