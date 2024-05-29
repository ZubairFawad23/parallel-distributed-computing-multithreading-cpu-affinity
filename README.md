# parallel-distributed-computing-multithreading-cpu-affinity
A project implementing parallel and distributed computing using multithreading and CPU affinity in C.

# Parallel and Distributed Computing with Multithreading and CPU Affinity

## Overview
This project demonstrates parallel and distributed computing using multithreading and CPU affinity in C. It includes two tasks: calculating square roots of array elements and summing elements of a 2D array, both implemented using threads.

## Instructions
### Compilation
To compile the programs, navigate to the `src` directory and use the following commands:
```bash
gcc -o task1 task1.c -lpthread -lm
gcc -o task2 task2.c -lpthread -lm
