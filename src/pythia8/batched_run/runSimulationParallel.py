import os
import subprocess
import time
from datetime import datetime
import re
import sys

if len(sys.argv) != 2:
    print("Usage: python3 runSimulationParallel.py <number_of_batches>")
    sys.exit(1)

num_batches = int(sys.argv[1])

# Define regex pattern to match batch directories
batch_pattern = re.compile(r"BATCH(\d{3})")

# Get the maximum existing batch number
existing_batches = [int(batch_pattern.match(d).group(1)) for d in os.listdir("results") if batch_pattern.match(d)]
max_batch_number = max(existing_batches) if existing_batches else 0

# Generate new batch names
new_batches = [f"BATCH{max_batch_number + i + 1:03d}" for i in range(num_batches)]

# Define variables for energies and PDF sets
executables = ["runSimulationParallel5tevCteq66", "runSimulationParallel5tevNnpdf23", "runSimulationParallel13tevCteq66", "runSimulationParallel13tevNnpdf23"]
energy_dirs = ["5TEV", "13TEV"]
pdf_sets = ["CTEQ66", "NNPDF23"]

for batch_dir in new_batches:
    # Create the necessary directory structure
    for energy in energy_dirs:
        for pdf in pdf_sets:
            result_dir = os.path.join("results", batch_dir, energy, pdf)
            sim_dir = os.path.join(result_dir, "sim")
            os.makedirs(sim_dir, exist_ok=True)

    # Define the text file to store timings
    timings_file = "batch_simulation_timings.txt"

    # Start time for the entire script
    start_time = time.time()
    start_time_readable = datetime.now().strftime("%a, %d %b %Y, %H:%M:%S")

    # Initialize the text file with a header
    with open(timings_file, "w") as f:
        f.write(f"Batch simulation started: {start_time_readable}\n")
        f.write("-------------------\n")

    # Compile all necessary executables in the current directory using make
    for executable in executables:
        print(f"Compiling {executable}.cc...")
        subprocess.run(["make", executable])

    # Move executables to their respective directories using regex
    for executable in executables:
        match = re.match(r'runSimulationParallel(\d+tev)(\w+)', executable)
        if match:
            energy = match.group(1).upper()
            pdf = match.group(2).upper()

            if "NNPDF23" in pdf:
                pdf = "NNPDF23"
            elif "CTEQ66" in pdf:
                pdf = "CTEQ66"

            result_dir = os.path.join("results", batch_dir, energy, pdf)
            sim_dir = os.path.join(result_dir, "sim")

            if os.path.isfile(executable):
                os.rename(executable, os.path.join(sim_dir, executable))
            else:
                print(f"Error: {executable} not found. Skipping...")
                continue

            # Copy additional files to the sim directory
            subprocess.run(["cp", "Makefile", sim_dir])
            config_file = f"{executable}.config.cc"
            if os.path.isfile(config_file):
                subprocess.run(["cp", config_file, sim_dir])

    # Run the executables sequentially using regex
    original_dir = os.getcwd()
    for executable in executables:
        match = re.match(r'runSimulationParallel(\d+tev)(\w+)', executable)
        if match:
            energy = match.group(1).upper()
            pdf = match.group(2).upper()

            if "NNPDF23" in pdf:
                pdf = "NNPDF23"
            elif "CTEQ66" in pdf:
                pdf = "CTEQ66"

            result_dir = os.path.join("results", batch_dir, energy, pdf)
            sim_dir = os.path.join(result_dir, "sim")
            
            # Change directory to sim and run the executable
            os.chdir(sim_dir)
            print(f"Running {executable} in {sim_dir}...")
            start_process_time = time.time()
            subprocess.run(["./" + executable])
            end_process_time = time.time()
            process_duration = int(end_process_time - start_process_time)
            
            with open(timings_file, "a") as f:
                f.write(f"Duration for {executable}: {process_duration} seconds\n")
                f.write("-------------------\n")
            
            os.chdir(original_dir)

    # End time for the entire script
    end_time = time.time()
    end_time_readable = datetime.now().strftime("%a, %d %b %Y, %H:%M:%S")

    # Calculate the total duration
    total_duration = int(end_time - start_time)

    # Format the duration in days, hours, minutes, and seconds
    duration_days = total_duration // 86400
    duration_hours = (total_duration % 86400) // 3600
    duration_minutes = (total_duration % 3600) // 60
    duration_seconds = total_duration % 60

    # Format the total run time in hh:mm:ss
    total_run_time = f"{duration_hours:02}:{duration_minutes:02}:{duration_seconds:02}"

    # Record the total duration and end time in the text file
    with open(timings_file, "a") as f:
        f.write(f"Batch simulation ended: {end_time_readable}\n")
        f.write("-------------------\n")
        f.write(f"Simulation duration: {duration_days} days, {duration_hours} hours, {duration_minutes} minutes, {duration_seconds} seconds\n")
        f.write(f"Total run time: {total_run_time} (hh:mm:ss)\n")

    # Move the timings file to the batch directory
    os.rename(timings_file, os.path.join("results", batch_dir, timings_file))
