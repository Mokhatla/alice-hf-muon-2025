#!/bin/zsh

# Define the specific executable to run and its directory
EXECUTABLE="runSimulationParallel13tevCteq66"
RESULT_DIR="results/BATCH555/13TEV/CTEQ66/sim"

# Define the output files
STDOUT_FILE="${RESULT_DIR}/stdout.log"
STDERR_FILE="${RESULT_DIR}/stderr.log"

# Change directory to the sim directory and run the executable
cd ${RESULT_DIR} || { echo "Error: Directory ${RESULT_DIR} does not exist."; exit 1; }
echo "Running ${EXECUTABLE} in ${RESULT_DIR}..."
./${EXECUTABLE} > "${STDOUT_FILE}" 2> "${STDERR_FILE}"