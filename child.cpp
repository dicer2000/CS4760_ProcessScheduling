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


using namespace std;

// Main - expecting arguments
int main(int argc, char* argv[])
{
    // This main area will only handle the processing
    // of the incoming arguments.

    string strLog =  "Child app by Brett Huffman for CMP SCI 4760";
    cout << endl << strLog << endl << endl;

    // Register SIGQUIT handling
    signal(SIGINT, sigQuitHandler);

    // Create a Semaphore to coordinate control
//    productSemaphores s(KEY_MUTEX, true, 1);


    // Pid used throughout child
    const pid_t nPid = getpid();
    cout << "Child PID: " << nPid << endl;

    // Seed the randomizer with the PID
    srand(time(0) ^ nPid);

    // Open the connection with the Message Queue
    // msgget creates a message queue 
    // and returns identifier 
    int msgid = msgget(KEY_MESSAGE_QUEUE, 0666 | IPC_CREAT); 
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

    // Loop until child process is stopped or it shuts down naturally
    while(true)
    {

        // Listen to shared memory and look for my Type => Which is my PID
        msgrcv(msgid, &message, sizeof(message), nPid, 0); 

        cout << "Child: from OSS: " << message.mesg_text << endl;

        // Send back to oss
        char lmess[] = "Hello Back At Ya\0";
        memcpy(message.mesg_text, lmess, strlen(lmess)+1 );
        message.mesg_type = OSS_MQ_TYPE;
        int n = msgsnd(msgid, &message, sizeof(message), 0);
        cout << "Child Result: " << errno << endl;

        sleep(2);
    }

/*
    // Argument processing
    int opt;
    // Go through each parameter entered and
    // prepare for processing
    while ((opt = getopt(argc, argv, "hs:l")) != -1) {
        switch (opt) {
            case 'h':
                show_usage(argv[0]);
                return EXIT_SUCCESS;
            case 'l':
                strLogFile = optarg;
                break;
            case 's':
                nNumberOfSeconds = atoi(optarg);
                break;
            case '?': // Unknown arguement                
                if (isprint (optopt))
                {
                    errno = EINVAL;
                    perror("Unknown option");
                }
                else
                {
                    errno = EINVAL;
                    perror("Unknown option character");
                }
                return EXIT_FAILURE;
            default:    // An bad input parameter was entered
                // Show error because a bad option was found
                perror ("oss: Error: Illegal option found");
                show_usage(argv[0]);
                return EXIT_FAILURE;
        }
    }
*/

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