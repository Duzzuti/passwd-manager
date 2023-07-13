/*
this module holds the application class which is the interface between the main function and the rest of the program
the run() method is the encapsulated main method
*/
#pragma once

#include <iostream>

#include "base.h"
#include "filehandler.h"

class App {
   private:
    std::string filePath;  // stores the path to the encryption file
    FileHandler FH;        // stores the FileHandler object

   private:
    // does most of the work if you call any of the isValid methods
    static bool isValidChar(const std::string mode) noexcept;
    // this method is printing some start informations. If no filepath is set we create a new file in the current dir
    void printStart();
    std::string askForPasswd() const noexcept;                    // asks the user for a password and validates the input format
    FModes askForFileMode() const noexcept;                       // asks the user for a file mode and validates the input format
    HModes askForHashMode() const noexcept;                       // asks the user for a hash mode and validates the input format
    CHModes askForChainHashMode() const noexcept;                 // asks the user for a chain hash mode and validates the input format
    u_int64_t askForIters(const std::string msg) const noexcept;  // asks the user for an iteration number and validates the input format
    Bytes askForHeader() const;                                   // asks the user for data that is put into a header for the file
   public:
    App();
    bool run();                                                                                          // main function of the app, starts the app
    static bool isValidFileMode(const std::string mode, const bool accept_blank = false) noexcept;       // checks if the entered file mode is valid
    static bool isValidHashMode(const std::string mode, const bool accept_blank = false) noexcept;       // checks if the entered hash mode is valid
    static bool isValidChainHashMode(const std::string mode, const bool accept_blank = false) noexcept;  // checks if the entered chain hash mode is valid
    // checks if the entered number (u_int64_t) is valid, underflow upper bound to get the max long value
    static bool isValidNumber(const std::string number, const bool accept_blank = false, const u_int64_t lower_bound = 0, const u_int64_t upper_bound = -1) noexcept;
};
