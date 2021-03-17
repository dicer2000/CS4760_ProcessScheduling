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
#include "oss.h"

using namespace std;

// SIGINT handling
volatile sig_atomic_t sigIntFlag = 0;
void sigintHandler(int sig){ // can be called asynchronously
  sigIntFlag = 1; // set flag
}



// ossProcess - Process to start oss process.
int ossProcess(string strLogFile, int nMaxSeconds)
{
    // Important items
    struct OssHeader* ossHeader;
    struct OssItem* ossItemQueue;

    // Check Input and exit if a param is bad
    if(nMaxSeconds < 1)
    {
    errno = EINVAL;
    perror("OSS: Unknown option");
    return EXIT_FAILURE;
    }

    // Register SIGINT handling
    signal(SIGINT, sigintHandler);
    bool isKilled = false;

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
        ossItemQueue[i].itemValue = 0.0f;
    }

    memset(usageArray, 0, sizeof(usageArray));

    for(int i=0;i < 18; i++)
    {
        cout << std::bitset<8>(usageArray[i]) << " ";
    }
    cout << endl;
    
    setBitmapByte(usageArray, 10, true);

    cout << "Val: " << getBitmapByte(usageArray, 10) << endl;

    for(int i=0;i < 18; i++)
    {
        cout << std::bitset<8>(usageArray[i]) << " ";
    }
    cout << endl;

    setBitmapByte(usageArray, 10, false);

    cout << "Val: " << getBitmapByte(usageArray, 10) << endl;

    for(int i=0;i < 18; i++)
    {
        cout << std::bitset<8>(usageArray[i]) << " ";
    }
    cout << endl;

    toggleByte(usageArray, 10);

    cout << "Val: " << getBitmapByte(usageArray, 10) << endl;

    for(int i=0;i < 18; i++)
    {
        cout << std::bitset<8>(usageArray[i]) << " ";
    }
    cout << endl;

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

    // Success!
    return EXIT_SUCCESS;
}


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

