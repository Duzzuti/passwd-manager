#pragma once
#pragma once
#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <cstdlib>
#include <string>
#include <sys/stat.h>
#include <filesystem>

class FileHandler{
private:
    std::string appDataDir;
    std::string fileName;
private:
    void getAppDataDir(); // Get the path to the directory where the application can store data
    void createAppDataDir(); // Create the application data directory if it doesn't exist
public:
    FileHandler();
    bool setFilePath(std::string path);
    std::string getFilePath() const noexcept;
};

#endif //FILEHANDLER_H