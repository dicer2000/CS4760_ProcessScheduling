/********************************************
 * oss App - Process Scheduling (oss) Application
 * This is the main functionality of the oss
 * application.  It is called by the oss_main.cpp
 * file.
 * 
 * Brett Huffman
 * CMP SCI 4760 - Project 4
 * Due Mar 29, 2021
 * oss CPP file for oss project
 ********************************************/

#include <iostream>
#include <string.h>
#include <vector>
#include <unistd.h>
#include "sharedStructures.h"
#include "productSemaphores.h"
#include "bitmapper.h"
#include "oss.h"

using namespace std;

// SIGINT handling
volatile sig_atomic_t sigIntFlag = 0;
void sigintHandler(int sig){ // can be called asynchronously
  sigIntFlag = 1; // set flag
}



// ossProcess - Process to start oss process.
int ossProcess(string strLogFile, int timeInSecondsToTerminate)
{
    // Important items
    struct OssHeader* ossHeader;
    struct OssItem* ossItemQueue;
    int wstatus;

    // Start Time for time Analysis
    time_t secondsStart;
    uint systemClockSeconds = 0;
    uint systemClockNanoseconds = 0;

    // Bitmap object for keeping track of children
    bitmapper bm(QUEUE_LENGTH);

    // Debug view
    bm.debugPrintBits();

    // Check Input and exit if a param is bad
    if(timeInSecondsToTerminate < 1)
    {
        errno = EINVAL;
        perror("OSS: Unknown option");
        return EXIT_FAILURE;
    }

    // Register SIGINT handling
    signal(SIGINT, sigintHandler);
    bool isKilled = false;
    bool isShutdown = false;
    bool startProcesses = true;

    // Create a Semaphore to coordinate control
//    productSemaphores s(KEY_MUTEX, true, 1);

    // Setup Message Queue Functionality
    // Note: The oss app will always have a type of 1
    int msgid = msgget(KEY_MESSAGE_QUEUE, 0666 | IPC_CREAT); 
    if (msgid == -1) {
        perror("OSS: Error creating Message Queue");
        exit(EXIT_FAILURE);
    }

    // Setup shared memory
    // allocate a shared memory segment with size of 
    // Product Header + entire Product array
    int memSize = sizeof(OssHeader) + sizeof(OssItem) * QUEUE_LENGTH;
    shm_id = shmget(KEY_SHMEM, memSize, IPC_CREAT | IPC_EXCL | 0660);
    if (shm_id == -1) {
        perror("OSS: Error allocating shared memory");
        exit(EXIT_FAILURE);
    }

    // attach the shared memory segment to our process's address space
    shm_addr = (char*)shmat(shm_id, NULL, 0);
    if (!shm_addr) { /* operation failed. */
        perror("OSS: Error attaching shared memory");
        exit(EXIT_FAILURE);
    }
    // Get the queue header
    ossHeader = (struct OssHeader*) (shm_addr);
    // Get our entire queue
    ossItemQueue = (struct OssItem*) (shm_addr+sizeof(int)+sizeof(OssHeader));

    //  productItemQueue = (struct ProductItem*) (shm_addr+sizeof(int));
    // Fill the product header
    ossHeader->systemClockSeconds = 0;
    ossHeader->systemClockNanoseconds = 0;

    // Set all items in queue to empty
    for(int i=0; i < QUEUE_LENGTH; i++)
    {
        ossItemQueue[i].readyToProcess = false;
        ossItemQueue[i].pidAssigned = 0;
    }


    int TestPID = -1;
    long TestCounter = 0;
startProcesses = true;
    // Start of main loop that will do the following
    // - Create new processes on avg of 1 sec intervals
    // - schedule processes to run on round-robin basis
    // - handle shutdown of processes
    // - maintiain statistics of each process
    // - assorted other misc items
    while(!isKilled && !isShutdown)
    {
        // Get Exclusive Control
//        s.Wait();

        // Check bitmap for room to make new processes
        if(startProcesses && TestPID < 0)
        {
            // Check if there is room for new processes
            // in the bitmap structure
            int nIndex = 0;
            for(;nIndex < QUEUE_LENGTH; nIndex++)
            {
                if(!bm.getBitmapBits(nIndex))
                {
                    // Found one.  Create new process
                    int newPID = forkProcess(ChildProcess, "logFile", nIndex);
    TestPID = newPID;
    cout << "NewPID: " << TestPID << endl;
                    // Setup Shared Memory for processing
                    ossItemQueue[nIndex].pidAssigned = newPID;
                    ossItemQueue[nIndex].readyToProcess = true;
                    // Set bit in bitmap
                    bm.setBitmapBits(nIndex, true);
                    // Log it
                    string strLogText = "OSS: Generating process with PID ";
                    strLogText.append(GetStringFromInt(newPID));
                    strLogText.append(" and putting it in queue ");
                    strLogText.append(GetStringFromInt(nIndex));
                    strLogText.append(" at time ");
                    strLogText.append(GetStringFromInt(systemClockSeconds));
                    strLogText.append(":");
                    strLogText.append(GetStringFromInt(systemClockNanoseconds));
                    WriteLogFile(strLogText, strLogFile);
                    break;
                }
            }
        }

        // Terminate the process if CTRL-C is typed
        // or if the max time-to-process has been exceeded
        // but only send out messages to kill once
        if((sigIntFlag || (time(NULL)-secondsStart) > timeInSecondsToTerminate) && !isKilled)
        {
            isKilled = true;
            // Send signal for every child process to terminate
            for(int nIndex=0;nIndex<QUEUE_LENGTH;nIndex++)
            {
                // Send signal to close if they are in-process
                if(bm.getBitmapBits(nIndex))
                    kill(ossItemQueue[nIndex].pidAssigned, SIGQUIT); 
            }

            // We have notified children to terminate immediately
            // then let program shutdown naturally -- that way
            // shared resources are deallocated correctly
            cout << endl;
            if(sigIntFlag)
            {
                errno = EINTR;
                perror("Killing processes due to ctrl-c signal");
            }
            else
            {
                errno = ETIMEDOUT;
                perror("Killing processes due to timeout");
            }
        }

        // Check for a PID
        // Note :: We use the WNOHANG to call waitpid without blocking
        // If it returns 0, it does not have a PID waiting
        int waitPID = waitpid(-1, &wstatus, WNOHANG | WUNTRACED | WCONTINUED);

        // No PIDs are in-process
        if (waitPID == -1)
        {
cout << "******In waitPID==-1" << endl;
            isShutdown = true;
            break;
        }

        // A PID Exited
        if (WIFEXITED(wstatus) && waitPID > 0)
        {
            // Find the PID and remove it from the bitmap
            for(int nIndex=0;nIndex<QUEUE_LENGTH;nIndex++)
            {
                if(ossItemQueue[nIndex].pidAssigned == waitPID)
                {
                    // Update the overall statistics
                    ossItemQueue[nIndex].procCtrlBlock.totalCPUTime = 0;
                    // Reset to start over
                    ossItemQueue[nIndex].readyToProcess = false;
                    ossItemQueue[nIndex].pidAssigned = 0;
                    bm.setBitmapBits(nIndex, false);
                    break;
                }
            }


        } else if (WIFSIGNALED(wstatus) && waitPID > 0) {
            cout << waitPID << " killed by signal " << WTERMSIG(wstatus) << endl;
        } else if (WIFSTOPPED(wstatus) && waitPID > 0) {
            cout << waitPID << " stopped by signal " << WTERMSIG(wstatus) << endl;
        } else if (WIFCONTINUED(wstatus) && waitPID > 0) {
            continue;
        }

cout << "****** Here *******" << endl;

        // *********** Testing ***************
        if(TestPID > 0 && TestCounter%4==0)
        {
cout << "Sending to PID: " << TestPID << endl;
            char lmess[] = "Hello\0";

            memcpy(message.mesg_text, lmess, strlen(lmess)+1 );
            message.mesg_type = TestPID;
            cout << message.mesg_text << endl;
            cout << message.mesg_type << endl;
            int n = msgsnd(msgid, &message, sizeof(message), 0);
            cout << "Result: " << errno << endl;

            msgrcv(msgid, &message, sizeof(message), OSS_MQ_TYPE, 0); 
            cout << "OSS: from child: " << message.mesg_text << endl;

        }
        TestCounter++;

        // Release Control
//        s.Signal();


        sleep(2);
    } // End of main loop



    // Breakdown shared memory
    // Dedetach shared memory segment from process's address space
    cout << endl;
    cout << "OSS: De-allocating shared memory" << endl;
    if (shmdt(shm_addr) == -1) {
        perror("OSS: Error detaching shared memory");
    }

    // De-allocate the shared memory segment.
    if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
        perror("OSS: Error deallocating shared memory ");
    }
    cout << "OSS: Shared memory De-allocated" << endl << endl;

    // Destroy the Message Queue
    msgctl(msgid,IPC_RMID,NULL);

    cout << "OSS: Message Queue De-allocated" << endl << endl;

    // Success!
    return EXIT_SUCCESS;
}


// ForkProcess - fork a process and return the PID
int forkProcess(string strProcess, string strLogFile, int nArrayItem)
{
        pid_t pid = fork();
        // No child made - exit with failure
        if(pid < 0)
        {
            // Signal to any child process to exit

            perror("OSS: Could not fork process");
            return EXIT_FAILURE;
        }
        // Child process here - Assign out it's work
        if(pid == 0)
        {
            // Execute child process without array arguements
            if(nArrayItem < 0)
              execl(strProcess.c_str(), strProcess.c_str(), strLogFile.c_str(), (char*)0);
            else
            {
              // Convert int to a c_str to send to exec
              string strArrayItem = GetStringFromInt(nArrayItem);
              execl(strProcess.c_str(), strProcess.c_str(), strArrayItem.c_str(), strLogFile.c_str(), (char*)0);
            }

            fflush(stdout); // Mostly for debugging -> tty wasn't flushing
            exit(EXIT_SUCCESS);    // Exit from forked process successfully
        }
        else
            return pid; // Returns the Parent PID
}

