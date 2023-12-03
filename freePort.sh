#!/bin/bash

# Define the port to be freed
PORT_TO_FREE=55555

# Find the process using the specified port
PID=$(lsof -t -i :$PORT_TO_FREE)

if [ -z "$PID" ]; then
    echo "No process found using port $PORT_TO_FREE"
else
    # Terminate the process
    echo "Terminating process $PID using port $PORT_TO_FREE"
    kill -9 $PID
fi
