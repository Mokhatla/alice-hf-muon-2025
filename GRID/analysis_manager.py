import os
import re
import subprocess
import time

# Define paths
run_analysis_path = os.path.expanduser("~/Documents/alice/GRID/MC/2024/LHC16K5B/runAnalysis.C")
list_files_script = os.path.expanduser("~/Documents/alice/GRID/list_monalisa_files.sh")
run_analysis_script = os.path.expanduser("~/Documents/alice/GRID/run_analysis_task.sh")
monalisa_home_path = "/alice/cern.ch/user/r/rmabitse/"

def get_error_jobs():
    result = subprocess.run(['alien.py', 'ps', '-E'], capture_output=True, text=True)
    
    if result.returncode != 0:
        print("Error fetching jobs in error state.")
        print(result.stderr)
        return []

    job_ids = []
    for line in result.stdout.strip().split('\n'):
        if line.strip():
            job_id = line.split()[1]
            if job_id.isdigit():
                job_ids.append(job_id)
    
    return job_ids

def resubmit_jobs(job_ids):
    if not job_ids:
        print("No jobs to resubmit.")
        return
    
    job_list = ','.join(job_ids)
    
    result = subprocess.run(['alien.py', 'resubmit', job_list], capture_output=True, text=True)
    
    if result.returncode != 0:
        print("Error resubmitting jobs.")
        print(result.stderr)
    else:
        print("Successfully resubmitted jobs:", job_list)

def extract_output_directory():
    """Extract the output directory name from the runAnalysis.C file."""
    with open(run_analysis_path, 'r') as file:
        content = file.read()
    
    # Regex to find the working directory line
    match = re.search(r'alienHandler->SetGridWorkingDir\("([^"]+)"\);', content)
    if match:
        output_directory = match.group(1)
        return output_directory
    else:
        print("Error: Output directory not found in runAnalysis.C")
        return None

def check_output_directory(output_directory):
    """Check if the output directory exists and if there are any files in it using alien.py ls -l."""
    try:
        result = subprocess.run([list_files_script, output_directory], capture_output=True, text=True, shell=True, executable="/bin/zsh")
        
        if result.returncode != 0:
            print(f"Error listing files in the output directory {output_directory}: {result.stderr}")
            return False
        
        # Check if the specific output directory exists in the listing
        directory_exists = False
        for line in result.stdout.strip().split('\n'):
            if re.search(rf'\b{output_directory}\b', line):
                directory_exists = True
                break
        
        if not directory_exists:
            print(f"Directory {output_directory} does not exist.")
            return False

        # Check if there are files in the output directory
        result = subprocess.run(['alien.py', 'ls', '-l', os.path.join(monalisa_home_path, output_directory)], capture_output=True, text=True, shell=True, executable="/bin/zsh")
        if result.returncode != 0:
            print(f"Error listing files in the specific output directory {output_directory}: {result.stderr}")
            return False

        if result.stdout.strip() == "":
            print(f"No files found in the specific output directory {output_directory}.")
            return False

        print(f"Files found in the specific output directory {output_directory}:\n{result.stdout}")
        return True

    except subprocess.CalledProcessError as e:
        print(f"Error checking output directory: {e}")
        return False

def extract_run_mode_and_merge_settings():
    """Extract the run mode and merge settings from the runAnalysis.C file."""
    with open(run_analysis_path, 'r') as file:
        content = file.read()

    # Extract the gridTest variable value
    grid_test_match = re.search(r'Bool_t gridTest = (kTRUE|kFALSE);', content)
    grid_test = grid_test_match.group(1) if grid_test_match else "kFALSE"

    # Determine the run mode based on the value of gridTest and if conditions
    if grid_test == "kTRUE":
        run_mode_match = re.search(r'if\s*\(gridTest\)\s*{.*?alienHandler->SetRunMode\("([^"]+)"\);', content, re.DOTALL)
        run_mode = run_mode_match.group(1) if run_mode_match else "test"
    else:
        run_mode_match = re.search(r'else\s*{[^}]*?alienHandler->SetRunMode\("([^"]+)"\);', content, re.DOTALL)
        run_mode = run_mode_match.group(1) if run_mode_match else "unknown"

    merge_stages_match = re.search(r'alienHandler->SetMaxMergeStages\((\d+)\);', content)
    merge_via_jdl_match = re.search(r'alienHandler->SetMergeViaJDL\((kTRUE|kFALSE)\);', content)
    
    merge_stages = int(merge_stages_match.group(1)) if merge_stages_match else None
    merge_via_jdl = merge_via_jdl_match.group(1) if merge_via_jdl_match else None

    return run_mode, merge_stages, merge_via_jdl

def run_analysis_task():
    """Run the analysis task."""
    try:
        result = subprocess.run([run_analysis_script], check=True, shell=True, executable="/bin/zsh")
        if result.returncode == 0:
            print("Analysis task completed successfully.")
        else:
            print("Error running the analysis task.")
    except subprocess.CalledProcessError as e:
        print(f"Error running the analysis task: {e}")


def determine_analysis_stage(run_mode, merge_stages, merge_via_jdl):
    """Determine the analysis stage based on run mode and merge settings."""
    if run_mode == "full" and merge_stages == 2 and merge_via_jdl == "kTRUE":
        return "analysis"
    elif run_mode == "terminate" and merge_stages == 2 and merge_via_jdl == "kTRUE":
        return "merge_stage_1"
    elif run_mode == "terminate" and merge_stages == 2 and merge_via_jdl == "kFALSE":
        return "merge_stage_2"
    else:
        return "unknown"
    
def main():
    manager_task_running = True
    job_resubmit_counters = {}

    while manager_task_running:
        print("Starting analysis task manager...")
        
        output_directory = extract_output_directory()
        print(f"Output directory: {output_directory}")

        if not output_directory:
            print("Exiting analysis manager.")
            break
        
        run_mode, merge_stages, merge_via_jdl = extract_run_mode_and_merge_settings()
        print(f"Run mode: {run_mode}")
        print(f"Merge stages: {merge_stages}")
        print(f"Merge via JDL: {merge_via_jdl}")

        analysis_stage = determine_analysis_stage(run_mode, merge_stages, merge_via_jdl)
        print(f"Analysis stage: {analysis_stage}")

        if analysis_stage in ["merge_stage_1", "merge_stage_2"]:
            print(f"Run mode: {run_mode}. Starting {analysis_stage} task...")
            merge_task_running = True
            
            while merge_task_running:
                # Get error jobs
                error_jobs = get_error_jobs()
                
                if error_jobs:
                    print(f"Found error jobs: {error_jobs}")
                
                # Update resubmit counters and resubmit jobs
                for job_id in error_jobs:
                    if job_id not in job_resubmit_counters:
                        job_resubmit_counters[job_id] = 0
                    if job_resubmit_counters[job_id] < 11:
                        resubmit_jobs([job_id])
                        job_resubmit_counters[job_id] += 1
                        print(f"Resubmitted job {job_id}, count: {job_resubmit_counters[job_id]}")
                    else:
                        print(f"Job {job_id} has been resubmitted 11 times and will not be resubmitted again.")

                # Check if all master jobs are done
                all_done = check_all_master_jobs_done()
                if all_done:
                    print("All master jobs are done. Ending merge task.")
                    merge_task_running = False
                    manager_task_running = False

                # Check if we should stop trying to resubmit error jobs
                elif all(job_resubmit_counters.get(job_id, 0) >= 11 for job_id in error_jobs):
                    print("All error jobs have been resubmitted more than 10 times. Ignoring error jobs and waiting for all master jobs to complete.")
                
                print("Sleeping for 10 minutes before the next check...")
                time.sleep(120)  # Sleep for 15 minutes
        elif not check_output_directory(output_directory) and analysis_stage == "analysis":
            print(f"Run mode: {run_mode}. Starting analysis task...")
            analysis_task_running = True
            #run_analysis_task()
            manager_task_running = False
        elif analysis_stage == "unknown":
            print("Unknown analysis stage. Exiting.")
            manager_task_running = False
        else:
            print("Setup error. Closing the analysis task manager...")
            manager_task_running = False

    print("Analysis task manager closing.")

def check_all_master_jobs_done():
    """Check if all master jobs are in the 'done' state."""
    result = subprocess.run(['alien.py', 'ps', '-M'], capture_output=True, text=True, shell=True, executable="/bin/zsh")
    
    if result.returncode != 0:
        print(f"Error checking master jobs: {result.stderr}")
        return False
    
    all_done = True
    for line in result.stdout.strip().split('\n'):
        if line.strip():
            status = line.split()[2]  # Assuming status is the third column
            if status != "D":
                all_done = False
                break

    return all_done


if __name__ == "__main__":
    main()