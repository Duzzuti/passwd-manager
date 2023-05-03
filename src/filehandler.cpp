#include "filehandler.h"

const std::string FileHandler::extension = ".enc";

void FileHandler::getAppDataDir(){
#if defined(_WIN32)
    // Get the user's app data directory on Windows
    char* appDataDirChar = nullptr;
    size_t len = 0;
    _dupenv_s(&appDataDirChar, &len, "APPDATA");
    if (appDataDirChar != nullptr) {
        this->appDataDir = appDataDirChar;
        free(appDataDirChar);
    }
#elif defined(__APPLE__)
    // Get the user's home directory on macOS
    char* homeDirChar = nullptr;
    size_t len = 0;
    _dupenv_s(&homeDirChar, &len, "HOME");
    if (homeDirChar != nullptr) {
        std::string homeDir = homeDirChar;
        free(homeDirChar);
        this->appDataDir = homeDir + "/Library/Application Support/Pman";
    }
#elif defined(__unix__)
    // Get the user's home directory on Unix-based systems
    char* homeDirChar = std::getenv("HOME");
    if (homeDirChar != nullptr) {
        std::string homeDir = homeDirChar;
        this->appDataDir = homeDir + "/.pman";
    }
#else
    #error Unsupported platform
#endif
}


void FileHandler::createAppDataDir(){
    this->getAppDataDir();
    std::string appDataDir = this->appDataDir;
    if (!appDataDir.empty()) {
        struct stat info;
        if (stat(appDataDir.c_str(), &info) != 0 || !(info.st_mode & S_IFDIR)) {
            // The directory doesn't exist, so create it
            mkdir(appDataDir.c_str(), 0700);
        }
    }
}

FileHandler::FileHandler(){
    this->createAppDataDir();
}

bool FileHandler::setFilePath(std::string path){
    std::filesystem::path fp{path};
    
    return std::filesystem::exists(fp);
}

std::string FileHandler::getFilePath() const noexcept{
    if(this->fileName.empty()){
        return std::string{};
    }
    return (std::filesystem::path(this->appDataDir) / (std::filesystem::path(this->fileName)));
}
