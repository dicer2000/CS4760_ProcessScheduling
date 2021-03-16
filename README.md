# Process Scheduling (oss) Application

Process Scheduling (oss) application by Brett Huffman for CMP SCI 4760 - Project 4 - V1.0

In this project, I simultate the process scheduling aspects of an OS.  Time-based scheduling is implemented in a main executable application that manages the execution of concurrent user processes.  Message Queues handle synchronization between the main application and it's concurrent user processes.

A log was kept of each day's activity.  It is found at the end of this README file.

A Git repository was maintained with a public remote found here: https://github.com/dicer2000/CS4760_ProcessScheduling.git

## Assumptions

There were some items I didn't understand about the project's operation.  Based on the feedback I did receive, I made these assumptions:

1. 


## Program Switches
The program can be invoked as:

```
oss [-h] 
oss [-s t] [-l f]
  -h Describe how the project should be run, then terminate.
  -s t Indicate how many maximum seconds before the system terminates
  -l f Specify a particular name for the log file
```

## Install
To install this program, clone it with git to the folder to which you want 
it saved.
```
git clone https://github.com/dicer2000/CS4760_ProcessScheduling.git
```
## Compile
To compile the master application, simply run the make command:
```
make
```
## Run
To run the program, use the oss command.  You can use any of the command line options listed in program switches area.

## Problems / Issues



## Work Log

- 3/15/2021 - Created project, Makefile, this readme file and got it all to compile; Setup constants, shared memory


*©2021 Brett W. Huffman*