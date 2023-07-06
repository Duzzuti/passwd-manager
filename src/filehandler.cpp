/*
this file contains the implementations of the FileHandler class
*/
#include "filehandler.h"

// set static extension var
const std::string FileHandler::extension = ".enc";  // encryption files (.enc)

FileHandler::FileHandler() {
    // constructs a file handler object by creating the app data file if it does not exist
    this->createAppDataDir();
    if (!this->isAppDataFile()) {
        this->createAppDataFile();
    }
    // filepath to the enc file is empty at default
    this->encryption_filepath = "";
    // get the app data setting for the enc file path
    std::optional<std::string> encryption_filepath = getAppSetting("filePath");
    if (encryption_filepath.has_value()) {
        // file path is set
        if (!std::filesystem::exists(encryption_filepath.value())) {
            // but it does not exist on the pc, remove this setting
            this->removeAppSetting("filePath");
        } else {
            // and it exists. Set this path
            this->encryption_filepath = encryption_filepath.value();
        }
    }
    // if there was no valid enc file path found, it is empty
}

void FileHandler::getAppDataDir() {
    // gets the app data directory depending on the os
#if defined(_WIN32)
    // Get the user's app data directory on Windows
    char* appDataDirChar = nullptr;
    size_t len = 0;
    _dupenv_s(&appDataDirChar, &len, "APPDATA");
    if (appDataDirChar != nullptr) {
        this->appDataDir = appDataDirChar;  // WORK, should be tested if an extended path has to be added
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

void FileHandler::createAppDataDir() {
    // creates the app data directory if it does not exist
    this->getAppDataDir();  // gets the directory
    std::string appDataDir = this->appDataDir;
    if (!appDataDir.empty()) {  // not yet created?
        struct stat info;
        if (stat(appDataDir.c_str(), &info) != 0 || !(info.st_mode & S_IFDIR)) {
            // The directory doesn't exist, so create it
            mkdir(appDataDir.c_str(), 0700);
        }
    }
}

void FileHandler::createAppDataFile() {
    // creates the app data file
    std::filesystem::path appdatapath = this->getAppDataFilePath();  // gets the file path
    if (!std::filesystem::exists(appdatapath)) {
        std::ofstream(appdatapath.c_str());  // create the file if it does not exist
    }
}

std::filesystem::path FileHandler::getAppDataFilePath() const noexcept {
    // gets the file path to the app data file by concatinating the dir path with the file name
    return std::filesystem::path(this->appDataDir) / std::filesystem::path(this->appDataName);
}

void FileHandler::resetAppData() const noexcept {
    // try to recover the filePath
    std::optional<std::string> set = this->getAppSetting("filePath");
    // clear the app data file
    std::ofstream app_of(this->getAppDataFilePath().c_str());
    if (set.has_value()) {
        if (std::filesystem::exists(set.value().c_str())) {
            // filePath does exist, save it again into app data
            app_of << "filePath"
                   << " " << set.value() << std::endl;
            return;
        }
    }
    // could not recover the filePath, the user has to be asked
    // WORK
}

bool FileHandler::removeAppSetting(const std::string setting_name) const {
    // remove a given setting from the app data
    if (!this->isAppDataFile()) {
        // could not found the app data file
        throw std::runtime_error("App data file not found");
    }
    std::fstream file(this->getAppDataFilePath().c_str());  // create the file stream
    std::stringstream file_content;                         // stores the data of the file
    std::string line;
    while (std::getline(file, line)) {
        // reads the file line by line
        std::istringstream iss(line);
        std::string setting, value;
        if (!(iss >> setting >> value)) {
            // if the line does not conatin two strings (setting, value pair)
            std::cout << "The AppDataFile is not in the right format" << std::endl;  // error occured, not in right format
            std::cout << "Do you wanna reset the appData (y/n): ";
            std::string tmp;
            std::cin >> tmp;
            if (tmp == "y") {
                // reset the app data file to get the right format
                this->resetAppData();
            }
            return false;  // cannot proceed due to incorect file format
        }
        std::cout << setting << "|" << value << std::endl;  // DEBUGONLY
        if (!(setting == setting_name)) {
            // a setting found that is not equal to the setting that should be deleted (we will save it)
            file_content << line << std::endl;
        }
    }
    std::ofstream write_file;
    // opens the file again in write mode
    write_file.open(this->getAppDataFilePath().c_str(), std::ofstream::out | std::ofstream::trunc);
    write_file << file_content.str();  // writes the file with the read content without the deleted setting
    return true;
}

std::optional<std::string> FileHandler::getAppSetting(const std::string setting_name) const {
    // gets the value for a given setting from the app data file
    if (!this->isAppDataFile()) {
        // could not found the app data file
        throw std::runtime_error("App data file not found");
    }
    std::fstream file(this->getAppDataFilePath().c_str());  // create the file stream
    std::string line;
    while (std::getline(file, line)) {
        // reads the file line by line
        std::istringstream iss(line);
        std::string setting, value;
        if (!(iss >> setting >> value)) {
            // if the line does not conatin two strings (setting, value pair)
            std::cout << "The AppDataFile is not in the right format" << std::endl;  // error occured, not in right format
            std::cout << "Do you wanna reset the appData (y/n): ";
            std::string tmp;
            std::cin >> tmp;
            if (tmp == "y") {
                // reset the app data file to get the right format
                this->resetAppData();
            }
            return {};  // cannot proceed due to incorect file format
        }
        std::cout << setting << "|" << value << std::endl;  // DEBUGONLY
        if (setting == setting_name) {
            // setting is set in the appdata, return the value
            return value;
        }
    }
    return {};  // setting is not set in the appdata file, return empty optional
}

bool FileHandler::isAppDataFile() const noexcept {
    // checks if the app data file exists on the system
    std::ifstream file(this->getAppDataFilePath().c_str());  // creates a stream
    return (bool)file;                                       // convert the filestream to a bool, returns false if the file does not exist
}

bool FileHandler::setAppSetting(const std::string setting_name, const std::string setting_value) const {
    /*
    sets a new app setting with the provided name and value
    APP SETTINGS
    for docs look into docs/appdata.md
    */
    if (!this->isAppDataFile()) {
        // could not found the app data file
        throw std::runtime_error("App data file not found");
    }
    std::fstream file(this->getAppDataFilePath().c_str());  // create the file stream
    std::stringstream file_content;                         // stores the data of the file
    std::string line;
    while (std::getline(file, line)) {
        // reads the app data file line by line
        std::istringstream iss(line);
        std::string setting, value;
        if (!(iss >> setting >> value)) {
            // if the line does not conatin two strings (setting, value pair)
            std::cout << "The AppDataFile is not in the right format" << std::endl;  // error occured, not in right format
            std::cout << "Do you wanna reset the appData (y/n): ";
            std::string tmp;
            std::cin >> tmp;
            if (tmp == "y") {
                // reset the app data file to get the right format
                this->resetAppData();
            }
            return false;  // cannot proceed due to incorect file format
        }
        std::cout << setting << "|" << value << std::endl;  // DEBUGONLY
        if (!(setting == setting_name)) {
            // setting was not set previously, we have to save it, drop if it is the setting we wanna change
            file_content << line << std::endl;
        }
    }
    // add the new setting to the file content stream
    file_content << setting_name << " " << setting_value << std::endl;

    std::ofstream write_file;
    // open the file again in write mode
    write_file.open(this->getAppDataFilePath().c_str(), std::ofstream::out | std::ofstream::trunc);
    write_file << file_content.str();  // writes the file again with the read content and the added setting
    return true;
}

bool FileHandler::setEncryptionFilePath(const std::string path) noexcept {
    // set a new encryption file path
    std::filesystem::path fp{path};
    bool exist = std::filesystem::exists(fp);  // checks whether the provided path exists
    if (exist) {
        // if it exists, change the path in the app data
        this->setAppSetting("filePath", path);
        this->encryption_filepath = path;
    }
    return exist;  // return the success bool
}

std::filesystem::path FileHandler::getEncryptionFilePath() const noexcept {
    // getter for the encryption filepath
    return this->encryption_filepath;
}

Bytes FileHandler::getFirstBytes(const int num) const {
    // reads the num first bytes of the encryption file
    if (this->encryption_filepath.empty()) {
        // no encryption file path set
        throw std::runtime_error("Encrypted filepath is empty");
    }
    std::ifstream file(this->encryption_filepath.c_str());  // create the file stream
    if (!file) {
        // the set encryption file does not exist on the system
        throw std::runtime_error("Encrypted file not found in filepath");
    }
    // get length of file:
    file.seekg(0, file.end);    // jump to the end
    int length = file.tellg();  // saves the position
    file.seekg(0, file.beg);    // jumps to the start again
    if (length < num) {
        // not enough characters to read (file is not long enough)
        throw std::length_error("File contains to few characters");
    }
    char buf[num];        // creates a buffer to hold the read bytes
    file.read(buf, num);  // reads into the buffer
    // transform the char array into an Bytes object
    Bytes ret = Bytes();
    for (char c : buf) {
        ret.addByte(static_cast<unsigned char>(c));  // cast from char to unsigned char
    }
    return ret;
}

Bytes FileHandler::getAllBytes() const {
    // reads all Bytes from the file
    if (this->encryption_filepath.empty()) {
        // no encryption file path set
        throw std::runtime_error("Encrypted filepath is empty");
    }
    std::ifstream file(this->encryption_filepath.c_str());  // create the file stream
    if (!file) {
        // the set encryption file does not exist on the system
        throw std::runtime_error("Encrypted file not found in filepath");
    }
    // get length of file:
    file.seekg(0, file.end);    // jump to the end
    int length = file.tellg();  // saves the position
    file.close();
    // gets the Bytes
    return this->getFirstBytes(length);
}
