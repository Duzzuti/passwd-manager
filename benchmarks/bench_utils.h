#include <sys/resource.h>

#include <iostream>

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

// Function to get current memory usage (RSS) in bytes
u_int64_t getMemoryUsage() {
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) != 0) {
        // Error handling if getrusage fails
        return -1;
    }

    // RSS value is in kilobytes, convert to bytes
    return static_cast<u_int64_t>(usage.ru_maxrss) * 1024;
}

/*
this code was taken from:
https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
*/

int parseLine(char* line) {
    // This assumes that a digit will be found and the line ends in " Kb".
    int i = strlen(line);
    const char* p = line;
    while (*p < '0' || *p > '9') p++;
    line[i - 3] = '\0';
    i = atoi(p);
    return i;
}

// int getVirtualMem(){ //Note: this value is in KB!
//     FILE* file = fopen("/proc/self/status", "r");
//     int result = -1;
//     char line[128];

//     while (fgets(line, 128, file) != NULL){
//         if (strncmp(line, "VmSize:", 7) == 0){
//             result = parseLine(line);
//             break;
//         }
//     }
//     fclose(file);
//     return result;
// }

int getPhysicalMem() {  // Note: this value is in KB!
    FILE* file = fopen("/proc/self/status", "r");
    int result = -1;
    char line[128];

    while (fgets(line, 128, file) != NULL) {
        if (strncmp(line, "VmRSS:", 6) == 0) {
            result = parseLine(line);
            break;
        }
    }
    fclose(file);
    return result;
}