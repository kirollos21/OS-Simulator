# OS Simulator

A simulator for core operating-system concepts, written in C. It reads a configuration file and a program metadata file, then simulates process scheduling and resource management, logging the run to the monitor and/or a file.

## Features

- Configurable scheduling and simulation parameters via `.cnf` config files
- Program metadata (`.mdf`) parsing into operation queues
- Process and resource management with simulated timing
- Logging to monitor, file, or both

## Build & run

```sh
gcc *.c -o sim
./sim config0.cnf
```

> Coursework project for an Operating Systems course.
