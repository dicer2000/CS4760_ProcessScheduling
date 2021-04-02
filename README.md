# Process Scheduling (oss) Application

Process Scheduling (oss) application by Brett Huffman for CMP SCI 4760 - Project 4 - V1.0

In this project, I simultate the process scheduling aspects of an OS.  Time-based scheduling is implemented in a main executable application that manages the execution of concurrent user processes.  Message Queues handle synchronization between the main application and it's concurrent user processes.

A log was kept of each day's activity.  It is found at the end of this README file.

A Git repository was maintained with a public remote found here: https://github.com/dicer2000/CS4760_ProcessScheduling.git

## Assumptions
There were some items I didn't understand about the project's operation.  Based on the feedback I did receive, I made these assumptions:

1. All times are processed and shown in Seconds:Nanoseconds
2. Processing stops after 3 real seconds or no processes remain running or if 100 processes already generated
3. Logging will happen for anything that is processed regardless of length
4. I've decided to use Shared Message Queue without IPC_NOWAIT.  That means that the sending process blocks until the receiver sends a message back to you.  I felt that best simulated real-life processing and made my code simpler.

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

The biggest problem experienced (so far) is getting all the timing to work properly.  I've had issues trying to get it correct.  If it's off, it seems to shut everything down prematurely.  So, I've found you have to tune it in to get it to work properly.

I also had problems getting the calls to shared messgaes correct.  At first, I had the size parameter sending the entire message struct size.  That had random results.  I had to re-read the man page many times to realize it was simply the size of the text message (not the entire struct).  Once I got that it started sending messages well.

## Work Log

- 3/15/2021 - Created project, Makefile, this readme file and got it all to compile; Setup constants, shared memory
- 3/16/2021 - Added to git; Made sure initial model is working on Hoare;
- 3/17/2021 - Created a Bitmap class for use; Research; Started creating main oos processing loop
- 3/18/2021 - Got MessageQueue working with single child; Research
- 3/19/2021 - Research; Debugging
- 3/20/2021 - Rewrote queue
- 3/21/2021 - Began testing with child; Added timing to child; testing
- 3/22/2021 - Research; Added Ready and Blocked queue to project
- 3/28/2021 - Testing
- 3/29/2021 - Added multiple processes
- 3/30/2021 - Debugging multiple processes; Trying to setup timing; Tuning timing; Debugging timing; Setting up unique logging
- 4/1/2021 - Started over (April Fools). Actually I'm just getting statistics to working correctly; debugging

*©2021 Brett W. Huffman*