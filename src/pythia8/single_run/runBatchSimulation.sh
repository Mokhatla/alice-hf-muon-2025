#!/bin/zsh

# Define executable names
EXECUTABLES=("runSimulationParallel5tevCteq66" "runSimulationParallel5tevNnpdf23" "runSimulationParallel13tevCteq66" "runSimulationParallel13tevNnpdf23")

# Define the text file to store timings
TIMINGS_FILE="simulation_timings.txt"

# Start time for the entire script
start_time=$(date +%s)  # Epoch time in seconds

# Initialize the text file with a header
echo "Simulation Run Times" > $TIMINGS_FILE
echo "-------------------" >> $TIMINGS_FILE

# Loop over each executable to run them sequentially
for executable in "${EXECUTABLES[@]}"; do
  # Start time for the current executable
  process_start_time=$(date +%s)

  # Run the executable and time its duration
  # Use 'time' to measure how long the process takes
  echo "Running $executable..." >> $TIMINGS_FILE
  { time ./$executable; } 2>> $TIMINGS_FILE

  # End time for the current executable
  process_end_time=$(date +%s)

  # Calculate the duration for the current executable
  process_duration=$((process_end_time - process_start_time))
  
  # Record the duration in the text file
  echo "Duration for $executable: $process_duration seconds" >> $TIMINGS_FILE
  echo "-------------------" >> $TIMINGS_FILE
done

# End time for the entire script
end_time=$(date +%s)

# Calculate the total duration
total_duration=$((end_time - start_time))

# Record the total duration in the text file
echo "Total duration for all simulations: $total_duration seconds" >> $TIMINGS_FILE
