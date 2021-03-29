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
#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <string.h>

#include <stdarg.h>  // For va_start, etc.
#include <memory>    // For std::unique_ptr

//***************************************************
// Structures
//***************************************************
struct ProcessControlBlock {        // Process Control Block
    uint totalCPUTime;
    uint totalSystemTime;
    uint timeUsedLastBurst;
    uint localSimulatedPID;
    uint processPriority;
    uint blockTimeSeconds;
    uint blockTimeNanoseconds;
    uint blockedUntilSeconds;
    uint blockedUntilNanoseconds;
};

struct OssHeader {
    int systemClockSeconds;     // System Clock - Seconds
    int systemClockNanoseconds; // System Clock - Nanoseconds
    
};

struct OssItem {
    ProcessControlBlock  PCB;
    int  pidAssigned;
    bool bReadyToProcess;
};

const key_t KEY_SHMEM = 0x54320;  // Shared key
int shm_id; // Shared Mem ident
char* shm_addr;

//***************************************************
// Message Queue
//***************************************************
const key_t KEY_MESSAGE_QUEUE = 0x54324;

// Structure for message queue 
struct message {
    long type;
    char text[20];
} msg;

const long OSS_MQ_TYPE = 1;

//***************************************************
// Important Program Constants
//***************************************************

// The size of our product queue
const int QUEUE_LENGTH = 18;
const char* ChildProcess = "./child";
const int maxTimeBetweenNewProcsNS = 10;
const int maxTimeBetweenNewProcsSecs = 10;
const int fullTransactionTimeInNS = 10000000;
const float percentageCPU = 0.9f;   // Higher numbers give more CPU Processes

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

std::string formatLogItem(std::string program, int sec, int msec, std::string action)
{
    std::string retVal = program;
//    retVal.append("- ");
    retVal.append(GetStringFromInt(sec));
    retVal.append("s:");
    retVal.append(GetStringFromInt(sec));
    retVal.append("ms\t");
    retVal.append(action);
    retVal.append("\n");
    return retVal;
}

// Log file writing helper function
bool WriteLogFile(std::string& logString, std::string LogFile)
{
    // Open a file to write
    std::ofstream logFile (LogFile.c_str(), std::ofstream::out | std::ofstream::app);
    if (logFile.is_open())
    {
        // Get the current local time
//        logFile << GetTimeFormatted("").c_str();
        logFile << " " << logString.c_str() << std::endl;
        logFile.close();
        return true;
    }
    else
    {
        perror("Unable to write to log file");
        return false;
    }
}

void setBitmapByte(unsigned char* bitmap, int addr, bool value)
{
    if(value)
    {
        // Set the bit at this point in the bitmap
        bitmap[addr/8] |= (1 << (7 - (addr%8)));
    }
    else
    {
        // Clear the bit
        bitmap[addr/8] &= ~(1 << (7 - (addr%8)));
    }
}

bool getBitmapByte(unsigned char* bitmap, int addr)
{
    // returns true or false based on whether value
    // is set to 1 or 0 in bitmap
    return (bitmap[addr/8] & (1 << (7 - (addr%8))));
}

void toggleByte(unsigned char* bitmap, int addr)
{
    // Toggle the bit at this point in the bitmap
    bitmap[addr/8] ^= (1 << (7 - (addr%8)));
}

// Returns a random number between two values
int getRandomValue(int MinVal, int MaxVal)
{
    return rand()%MaxVal+MinVal;
}

// Returns a random true/false based on the probability passed
// Be sure to run srand first!
bool getRandomProbability(float ProbabilityOfTrue)
{
  return rand()%100 < (ProbabilityOfTrue * 100);
}

std::string string_format(const std::string fmt_str, ...) {
    int final_n, n = ((int)fmt_str.size()) * 2; /* Reserve two times as much as the length of the fmt_str */
    std::unique_ptr<char[]> formatted;
    va_list ap;
    while(1) {
        formatted.reset(new char[n]); /* Wrap the plain char array into the unique_ptr */
        strcpy(&formatted[0], fmt_str.c_str());
        va_start(ap, fmt_str);
        final_n = vsnprintf(&formatted[0], n, fmt_str.c_str(), ap);
        va_end(ap);
        if (final_n < 0 || final_n >= n)
            n += abs(final_n - n + 1);
        else
            break;
    }
    return std::string(formatted.get());
}

#endif // SHAREDSTRUCTURES_H