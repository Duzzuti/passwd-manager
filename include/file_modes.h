#pragma once
#ifndef FILEMODES_H
#define FILEMODES_H

#include <iostream>
#include "file_data.h"

class FileModes{
    /*
    static class for the file data modes.
    You can get infos about a file mode or check if it is valid
    The important thing is that you can get the corresponding FileData class for that file mode
    */
public:
    //gets some information about this file mode (what it does)
    static std::string getInfo(const unsigned char file_mode);
    //checks if the given mode is valid (in a certain range)
    static bool isModeValid(const unsigned char file_mode) noexcept;
    //gets the FileData class that matches with the given file mode
    //it can be used to perform data specific actions (i.e. to modify the plain data)
    static FileData* getFileDataClass(const unsigned char file_mode); 
};


#endif //FILEMODES_H