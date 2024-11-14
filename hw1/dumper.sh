#!/bin/bash

# Define the path to the arger executable
ARGER="./arger"

# Check if the arger executable exists
if [ ! -x "$ARGER" ]; then
    echo "Error: The executable '$ARGER' does not exist or is not executable."
    exit 1
fi

# Function to run the arger program with the given arguments and handle errors
run_arger() {
    echo "Running: $ARGER $*"
    "$ARGER" "$@"
    EXIT_CODE=$?

    case $EXIT_CODE in
        0)
            echo "Execution successful."
            ;;
        -1)
            echo "Error occurred: Invalid option or missing text."
            ;;
        *)
            echo "Unexpected error occurred with exit code $EXIT_CODE."
            ;;
    esac
}

# Run the arger program with different arguments
run_arger -u "hello world"
run_arger -l "HELLO WORLD"
run_arger -cap "hello world"
run_arger -invalid "some text"
run_arger -u
