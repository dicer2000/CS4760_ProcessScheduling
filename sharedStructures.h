/********************************************
 * sharedStructures - This is a file that
 * contains all the structures shared between
 * the oss & child processes.  It
 * contains library calls, the main structure
 * containing data, and semephores.
 * 
 * Brett Huffman
 * CMP SCI 4760 - Project 4
 * Due Mar 29, 2021
 * sharedStructures.h file for project
 ********************************************/
#ifndef SHAREDSTRUCTURES_H
#define SHAREDSTRUCTURES_H

#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <math.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>

//***************************************************
// Shared Memory structures
//***************************************************
struct OssHeader {
    int systemClockSeconds;     // System Clock - Seconds
    int systemClockNanoseconds; // System Clock - Nanoseconds
    
};
struct OssItem {
    float itemValue;        // The actual "Product" being returned 
                            // from the Producer - A little Easter Egg
    bool readyToProcess;    // Ready to Process
};

const key_t KEY_SHMEM = 0x54320;  // Shared key
int shm_id; // Shared Mem ident
char* shm_addr;

//***************************************************
// Product Semaphores
//***************************************************
const key_t KEY_MUTEX = 0x54321;
const key_t KEY_EMPTY = 0x54322;
const key_t KEY_FULL = 0x54323;

struct shmseg {
   int cntr;
   int write_complete;
   int read_complete;
};

//***************************************************
// Important Program Constants
//***************************************************

// The size of our product queue
const int QUEUE_LENGTH = 18;
const char* ChildProcess = "./child";
const int maxTimeBetweenNewProcsNS = 10;
const int maxTimeBetweenNewProcsSecs = 10;

/***************************************************
 * Helper Functions
 * *************************************************/
// For time formatting used throughout both programs
std::string GetTimeFormatted(const char* prePendString)
{
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[10];
    
    // Get time
    time (&rawtime);
    timeinfo = localtime (&rawtime);

    // Format time for HH:MM:SS
    strftime (buffer,80,"%T",timeinfo);

    std::string strReturn = prePendString;
    strReturn.append(buffer);
    return strReturn;
}

// Log file writing helper function
bool WriteLogFile(std::string& logString, std::string LogFile)
{
    // Open a file to write
    std::ofstream logFile (LogFile.c_str(), std::ofstream::out | std::ofstream::app);
    if (logFile.is_open())
    {
        // Get the current local time
//        string 
        logFile << GetTimeFormatted("").c_str();
        logFile << " " << logString.c_str();
        logFile << std::endl;
        logFile.close();
        return true;
    }
    else
    {
        perror("Unable to write to log file");
        return false;
    }
}

// Returns a string from an int
std::string GetStringFromInt(const int nVal)
{
    int length = snprintf( NULL, 0, "%d", nVal);
    char* sDep = (char*)malloc( length + 1 );
    snprintf( sDep, length + 1, "%d", nVal);
    std::string strFinalVal = sDep;                    
    free(sDep);
    return strFinalVal;
}

void setBitmapByte(unsigned char* bitmap, int addr, bool value)
{

}

bool getBitmapByte(unsigned char* bitmap, int addr)
{
    // returns true or false based on whether value
    // is set to 1 or 0 in bitmap
    return (bitmap[addr/8] & (1 << (7 - (addr%8))));
}

#endif // SHAREDSTRUCTURES_H