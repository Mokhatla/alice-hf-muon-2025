#!/bin/zsh

# Prompt for the batch name
echo "Enter batch number (e.g., 001): "
read BATCH

BATCH_DIR="BATCH${BATCH}"

# Define variables for energies and PDF sets
# Define executable names
EXECUTABLES=("runSimulationParallel5tevCteq66" "runSimulationParallel5tevNnpdf23" "runSimulationParallel13tevCteq66" "runSimulationParallel13tevNnpdf23")
ENERGY_PDFS=("5tevNnpdf23" "13tevNnpdf23" "5tevCteq66" "13tevCteq66")
ENERGY_DIRS=("5TEV" "13TEV")
PDF_SETS=("NNPDF23" "CTEQ66")

# Define the text file to store timings
TIMINGS_FILE="simulation_timings.txt"

# Create the necessary directory structure
for ENERGY_PDF in "${ENERGY_PDFS[@]}"; do
  #ENERGY=$(echo $ENERGY_PDF | grep -oE '[0-9]+tev' | tr '[:lower:]' '[:upper:]')
  #PDF=$(echo $ENERGY_PDF | grep -oE '[a-zA-Z0-9]+$')
  ENERGY=$(echo $ENERGY_PDF | cut -d'_' -f1)
  PDF=$(echo $ENERGY_PDF | cut -d'_' -f2)

  RESULT_DIR="results/${BATCH_DIR}/${ENERGY}/${PDF}"
  SIM_DIR="${RESULT_DIR}/sim"
  mkdir -p "${SIM_DIR}"

  RESULT_DIR="results/${BATCH_DIR}/${ENERGY}/${PDF}"
  SIM_DIR="${RESULT_DIR}/sim"
  mkdir -p "${SIM_DIR}"
done

# Start time for the entire script
start_time=$(date +%s)  # Epoch time in seconds
start_time_readable=$(date "+%a, %d %b %Y, %H:%M:%S")

# Initialize the text file with a header
echo "Batch simulation started: ${start_time_readable}" > $TIMINGS_FILE
echo "-------------------" >> $TIMINGS_FILE

# Compile all necessary executables in the current directory using make
for ENERGY_PDF in "${ENERGY_PDFS[@]}"; do
  EXECUTABLE="runSimulationParallel${ENERGY_PDF}"
  echo "Compiling ${EXECUTABLE}.cc..."
  make ${EXECUTABLE}
done

# Move executables to their respective directories
for ENERGY_PDF in "${ENERGY_PDFS[@]}"; do
  # ENERGY=$(echo $ENERGY_PDF | grep -oE '[0-9]+tev' | tr '[:lower:]' '[:upper:]')
  #PDF=$(echo $ENERGY_PDF | grep -oE '[a-zA-Z0-9]+$')
  ENERGY=$(echo $ENERGY_PDF | cut -d'_' -f1)
  PDF=$(echo $ENERGY_PDF | cut -d'_' -f2)

  EXECUTABLE="runSimulationParallel${ENERGY_PDF}"
  RESULT_DIR="results/${BATCH_DIR}/${ENERGY}/${PDF}"
  SIM_DIR="${RESULT_DIR}/sim"

  # Move the executable to the correct sim directory
  if [[ -f ${EXECUTABLE} ]]; then
    mv ${EXECUTABLE} ${SIM_DIR}/
  else
    echo "Error: ${EXECUTABLE} not found. Skipping..."
    continue
  fi

  # Copy additional files to the sim directory
  cp "Makefile" "${SIM_DIR}"
  # If config file exists
  if [[ -f "${EXECUTABLE}.config.cc" ]]; then
    cp "${EXECUTABLE}.config.cc" "${SIM_DIR}"
  fi
done

# Run the executables sequentially
for ENERGY_PDF in "${ENERGY_PDFS[@]}"; do
  ENERGY=$(echo $ENERGY_PDF | grep -oE '[0-9]+tev' | tr '[:lower:]' '[:upper:]')
  PDF=$(echo $ENERGY_PDF | grep -oE '[a-zA-Z0-9]+$')

  EXECUTABLE="runSimulationParallel${ENERGY_PDF}"
  RESULT_DIR="results/${BATCH_DIR}/${ENERGY}/${PDF}"
  SIM_DIR="${RESULT_DIR}/sim"
  
  STDOUT_FILE="${RESULT_DIR}/stdout.log"
  STDERR_FILE="${RESULT_DIR}/stderr.log"
  
  # Change directory to sim and run the executable
  cd ${SIM_DIR}
  echo "Running ${EXECUTABLE} in ${SIM_DIR}..."
  { time ./${EXECUTABLE} > "${STDOUT_FILE}" 2> "${STDERR_FILE}"; } 2>> $TIMINGS_FILE
  cd -
done

# End time for the entire script
end_time=$(date +%s)
end_time_readable=$(date "+%a, %d %b %Y, %H:%M:%S")

# Calculate the total duration
total_duration=$((end_time - start_time))

# Format the duration in days, hours, minutes, and seconds
duration_days=$((total_duration / 86400))
duration_hours=$(( (total_duration % 86400) / 3600 ))
duration_minutes=$(( (total_duration % 3600) / 60 ))
duration_seconds=$((total_duration % 60))

# Format the total run time in hh:mm:ss
total_run_time=$(awk -v t=$total_duration 'BEGIN{t=int(t); printf("%02d:%02d:%02d", t/3600, (t%3600)/60, t%60)}')

# Record the total duration and end time in the text file
echo "Batch simulation ended: ${end_time_readable}" >> $TIMINGS_FILE
echo "-------------------" >> $TIMINGS_FILE
echo "Simulation duration: ${duration_days} days, ${duration_hours} hours, ${duration_minutes} minutes, ${duration_seconds} seconds" >> $TIMINGS_FILE
echo "Total run time: ${total_run_time} (hh:mm:ss)" >> $TIMINGS_FILE

# Move the timings file to the batch directory
mv $TIMINGS_FILE "results/${BATCH_DIR}/"