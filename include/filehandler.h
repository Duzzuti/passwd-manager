#pragma once
#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <iostream>
#include <cstdlib>
#include <string>
#include <sys/stat.h>
#include <filesystem>
#include <fstream>
#include <optional>

#include "bytes.h"

class FileHandler
{
    /*
    this class is the interface between the ui, the program logic and the files
    two files are important: The appdata file and the encryption file
    Data that should persist is saved into the app data file of the os
    The actual file with the encrypted content is the encryption file.
    */
private:
    std::string appDataDir;                     // stores the path to the appData directory
    const std::string appDataName = "data.txt"; // stores the name of the app data file
    std::string fileName;                       // stores the name of the encryption file
    std::filesystem::path encryption_filepath;  // stores the path to the encryption file
public:
    const static std::string extension; // stores the used file extension
private:
    void getAppDataDir();     // Get the path to the directory where the application can store data (app data dir)
    void createAppDataDir();  // Create the application data directory if it doesn't exist
    void createAppDataFile(); // creates the app data file if it does not exist
    // sets a new app data entry with a name and a value (writes into app data file), returns a success bool
    bool setAppSetting(const std::string setting_name, const std::string setting_value) const;
    // removes a app data entry by its name, returns a success bool
    bool removeAppSetting(const std::string setting_name) const;
    bool isAppDataFile() const noexcept;    //checks if the app data file exists under the stored path
    //gets the value of an given setting from the app data file. If it does not exist return an empty optional
    std::optional<std::string> getAppSetting(const std::string setting_name) const;
    std::filesystem::path getAppDataFilePath() const noexcept;  //gets the full path to the app data file
    void resetAppData() const noexcept; //resets the app data file (if there were errors)

public:
    //tries to get the encryption file path (if not encryption_filepath will be empty), creates app data file
    FileHandler();
    //sets a new filepath to an encryption file, but only if this file exists (returns success bool)
    bool setEncryptionFilePath(const std::string path) noexcept;
    std::string getEncryptionFilePath() const noexcept; //gets the path to the encryption file
    Bytes getFirstBytes(const int num) const;   //reads the first num Bytes from the encryption file
};

#endif // FILEHANDLER_H