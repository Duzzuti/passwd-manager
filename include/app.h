/*
this module holds the application class which is the interface between the main function and the rest of the programm
the run() method is the encapsulated main method
*/
#pragma once
#ifndef APP_H
#define APP_H

#include <iostream>
#include "filehandler.h"

class App{
private:
    std::string filePath;   //stores the path to the encryption file
    FileHandler FH;         //stores the FileHandler object

private:
    //this method is printing some start informations. If no filepath is set we create a new file in the current dir
    void printStart();
    std::string askForPasswd() const noexcept;      //asks the user for a password and validates the input format
    unsigned char askForFileMode() const noexcept;  //asks the user for a file mode and validates the input format
    unsigned char askForHashMode() const noexcept;  //asks the user for a hash mode and validates the input format
    unsigned char askForChainHashMode() const noexcept;         //asks the user for a chain hash mode and validates the input format
    unsigned long askForIters(std::string msg) const noexcept;  //asks the user for an iteration number and validates the input format
    Bytes askForHeader() const;     //asks the user for data that is put into a header for the file
public:
    App();
    bool run();
    static bool isValidFileMode(std::string mode, bool accept_blank=false) noexcept;
    static bool isValidHashMode(std::string mode, bool accept_blank=false) noexcept;
    static bool isValidChainHashMode(std::string mode, bool accept_blank=false) noexcept;
    static bool isValidNumber(std::string number, bool accept_blank=false, unsigned long lower_bound=0, unsigned long upper_bound=-1) noexcept;
};

#endif //APP_H