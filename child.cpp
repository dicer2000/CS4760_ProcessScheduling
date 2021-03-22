/********************************************
 * child - Process Scheduling (oss) Application
 * This is the child process called by the oos
 * application.
 * 
 * Brett Huffman
 * CMP SCI 4760 - Project 4
 * Due Mar 29, 2021
 * child CPP file for oss project
 ********************************************/

#include <iostream>
#include <unistd.h>
#include "sharedStructures.h"
#include "productSemaphores.h"
#include <fstream>
#include <stdlib.h>
#include <time.h>

// Forward declarations
static void show_usage(std::string);

// SIGQUIT handling
volatile sig_atomic_t sigQuitFlag = 0;
void sigQuitHandler(int sig){ // can be called asynchronously
  sigQuitFlag = 1; // set flag
}

// Enum for I/O vs CPU Bound Process
enum ChildType { IO, CPU };

using namespace std;

// Main - expecting arguments
int main(int argc, char* argv[])
{
    // This main area will only handle the processing
    // of the incoming arguments.

    string strLog =  "Child app by Brett Huffman for CMP SCI 4760";
    cout << endl << strLog << endl << endl;

    // Check incoming arguements
    if(argc < 2)
    {
        cout << "Args: " << argv[0] << endl;
        perror("Child: Incorrect argument found");
        exit(EXIT_FAILURE);
    }
    // Get the incoming Queue ID of the process
    const int nItemToProcess = atoi(argv[1]);

    // And the log file string
    string strLogFile = argv[2];

    // Register SIGQUIT handling
    signal(SIGINT, sigQuitHandler);

    // Pid used throughout child
    const pid_t nPid = getpid();
    cout << "Child PID: " << nPid << endl;

    // Seed the randomizer with the PID
    srand(time(0) ^ nPid);

    // Determine processing type and cast to a ChildType
    // This probability (<.50 will generate more CPU)
    const ChildType childType = getRandomProbability(percentageCPU) ? IO : CPU;
    //cout << "********************" << childType << endl;

    // Open the connection with the Message Queue
    // msgget creates a message queue 
    // and returns identifier 
    int msgid = msgget(KEY_MESSAGE_QUEUE, IPC_CREAT | 0666); 
    if (msgid == -1) {
        perror("Child: Error creating Message Queue");
        exit(EXIT_FAILURE);
    }

    // Open the connection to shared memory
    // Allocate the shared memory
    // And get ready for read/write
    // Get a reference to the shared memory, if available
    int shm_id = shmget(KEY_SHMEM, 0, 0);
    if (shm_id == -1) {
        perror("Child: Could not successfully find Shared Memory");
        exit(EXIT_FAILURE);
    }

    // Read the memory size and calculate the array size
    struct shmid_ds shmid_ds;
    shmctl(shm_id, IPC_STAT, &shmid_ds);
    size_t realSize = shmid_ds.shm_segsz;

    // Now we have the size - actually setup with shmget
    shm_id = shmget(KEY_SHMEM, realSize, 0);
    if (shm_id == -1) {
        perror("Child: Could not successfully find Shared Memory");
        exit(EXIT_FAILURE);
    }

    // attach the shared memory segment to our process's address space
    shm_addr = (char*)shmat(shm_id, NULL, 0);
    if (!shm_addr) { /* operation failed. */
        perror("Child: Could not successfully attach Shared Memory");
        exit(EXIT_FAILURE);
    }

    // Get the queue header
    struct OssHeader* ossHeader = 
        (struct OssHeader*) (shm_addr);
    // Get our entire queue - HA! Got the struct to align right
    struct OssItem*ossItemQueue = 
        (struct OssItem*) (shm_addr+sizeof(OssHeader));

//    cout << "Child Data" << endl;
//    cout << "C " << ossHeader->systemClockSeconds << endl;
//    cout << "C " << ossHeader->systemClockNanoseconds << endl;
//    cout << "C " << ossItemQueue[nItemToProcess].pidAssigned << endl;
//    cout << "C " << ossItemQueue[nItemToProcess].PCB.totalCPUTime << endl;

    // Set as ready to process
//    ossItemQueue[nItemToProcess].bReadyToProcess =  true;

    // Loop until child process is stopped or it shuts down naturally
    while(!sigQuitFlag)
    {

        // Set probabilities for this round
        bool willInterrupt = false;
        bool willShutdown = false;
        int nanoSecondsToShutdown = getRandomValue(200, 450);
        int nanoSecondsToInterrupt = getRandomValue(200, 450);   // Only used if interrupt happens
        if(childType == CPU)
            // CPU Bound process - less likely to get interrupted
            willInterrupt = getRandomProbability(0.10f) ? true : false;
        else
            // IO Bound - much more likely to get interrupted
            willInterrupt = getRandomProbability(0.90f) ? true : false;


        msgrcv(msgid, (void *) &msg, sizeof(struct message) - sizeof(long), nPid, 0); 

        cout << "Child: from OSS: " << msg.text << endl;

        // Send back to oss
        if(1==1)
        {
            // The full transaction happened
//            char lmess[] = "Full\0";
            strcpy(msg.text, "Full");
//            memcpy(message.mesg_text, lmess, strlen(lmess)+1 );
        }
        else
        {
            // An I/O blocked, partial transaction happened
//            char lmess[] = "Block\0";
//            memcpy(message.mesg_text, lmess, strlen(lmess)+1 );
            strcpy(msg.text, "Block");

        }
        msg.type = OSS_MQ_TYPE;

        int n = msgsnd(msgid, (void *) &msg, sizeof(struct message) - sizeof(long), 0);
        
        cout << "O: nval: " << n << endl;
        cout << "O: Result: " << errno << endl;

/*
        // Listen to shared memory and look for my Type => Which is my PID
//        message.mesg_type = nPid;
//        cout << "C: MessType: " << message.mesg_type << " / " << nPid << endl;
        msgrcv(msgid, &message, sizeof(message), nPid, 0); 

        cout << "Child: from OSS: " << message.mesg_text << endl;
        cout << "C: MessType: " << message.mesg_type << " / " << nPid << endl;
        cout << "C: S: " << willShutdown << "  I: " << willInterrupt << endl;
sleep(2);
        // Send back to oss
        if(willShutdown)
        {
            mesg_buffer lMB;
            char lmess[] = "Shutdown\0";
            memcpy(lMB.mesg_text, lmess, strlen(lmess)+1 );
            ossItemQueue[nItemToProcess].PCB.totalCPUTime += nanoSecondsToShutdown;
            // Send the message
            lMB.mesg_type = OSS_MQ_TYPE;

            cout << "C: SD MessType: " << lMB.mesg_text << " / " << lMB.mesg_type << endl;

            int n = msgsnd(msgid, &lMB, sizeof(lMB), 0);
            cout << "C: Result: " << errno << endl;
            // Shutdown gracefully
            return EXIT_SUCCESS;
        }
        else if(willInterrupt)
        {
            // An interrupt happened
            mesg_buffer lMB;
            char lmess[] = "Block\0";
            memcpy(lMB.mesg_text, lmess, strlen(lmess)+1 );
            ossItemQueue[nItemToProcess].PCB.totalCPUTime += nanoSecondsToInterrupt;
            ossItemQueue[nItemToProcess].PCB.timeUsedLastBurst = nanoSecondsToInterrupt;
            ossItemQueue[nItemToProcess].PCB.blockTimeSeconds += getRandomValue(0, 5);
            ossItemQueue[nItemToProcess].PCB.blockTimeNanoseconds += getRandomValue(0, 1000);          
            lMB.mesg_type = OSS_MQ_TYPE;

            cout << "C: IN MessType: " << lMB.mesg_text << " / " << lMB.mesg_type << endl;

            int n = msgsnd(msgid, &lMB, sizeof(lMB), 0);
            cout << "C: Result: " << errno << endl;
        }
        else
        {
            // A full transaction happened
            mesg_buffer lMB;
            char lmess[] = "Full\0";
            memcpy(lMB.mesg_text, lmess, strlen(lmess)+1 );
            ossItemQueue[nItemToProcess].PCB.totalCPUTime += fullTransactionTimeInNS;
            ossItemQueue[nItemToProcess].PCB.timeUsedLastBurst = fullTransactionTimeInNS;
            lMB.mesg_type = OSS_MQ_TYPE;

            cout << "C: FL MessType: " << lMB.mesg_text << " / " << lMB.mesg_type << endl;

            int n = msgsnd(msgid, &lMB, sizeof(lMB), 0);
            cout << "C: Result: " << errno << endl;
        }
    */

    }



}


// Handle errors in input arguments by showing usage screen
static void show_usage(std::string name)
{
    std::cerr << std::endl
              << name << " - child app by Brett Huffman for CMP SCI 4760" << std::endl
              << std::endl
              << "Usage:\t" << name << " [-h]" << std::endl
              << "\t" << name << " [-s t] [-l f]" << std::endl
              << "Options:" << std::endl
              << "  -h   Describe how the project should be run, then terminate" << std::endl
              << "  -s t Indicate how many maximum seconds before the system terminates" << std::endl
              << "  -l f Specify a particular name for the log file (Default logfile)" << std::endl
              << std::endl << std::endl;
}