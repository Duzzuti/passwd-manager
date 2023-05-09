#pragma once
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

class FileHandler{
private:
    std::string appDataDir;
    const std::string appDataName = "data.txt";
    std::string fileName;
    std::filesystem::path encryption_filepath;
public:
    const static std::string extension;
private:
    void getAppDataDir(); // Get the path to the directory where the application can store data
    void createAppDataDir(); // Create the application data directory if it doesn't exist
    void createAppDataFile();   //creates the app data file if it does not exist
    bool setAppSetting(std::string setting_name, std::string setting_value) const;
    bool removeAppSetting(std::string setting_name) const;
    bool isAppDataFile() const noexcept;
    std::optional<std::string> getAppSetting(std::string setting_name) const;
    std::filesystem::path getAppDataFilePath() const noexcept;
    void resetAppData() const noexcept;
public:
    FileHandler();
    bool setEncryptionFilePath(std::string path) noexcept;
    std::string getEncryptionFilePath() const noexcept;
    Bytes getFirstBytes(int num) const;
};

#endif //FILEHANDLER_H