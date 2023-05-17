#include "filehandler.h"

const std::string FileHandler::extension = ".enc";

FileHandler::FileHandler(){
    this->createAppDataDir();
    if(!this->isAppDataFile()){
        this->createAppDataFile();
    }
    this->encryption_filepath = "";
    std::optional<std::string> encryption_filepath = getAppSetting("filePath");
    if(encryption_filepath.has_value()){
        if(!std::filesystem::exists(encryption_filepath.value())){
            this->removeAppSetting("filePath");
        }else{
            this->encryption_filepath = encryption_filepath.value();
        }
    }

}

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

void FileHandler::createAppDataFile(){
    std::filesystem::path appdatapath = this->getAppDataFilePath();
    if(!std::filesystem::exists(appdatapath)){
        std::ofstream(appdatapath.c_str());
    }
}

std::filesystem::path FileHandler::getAppDataFilePath() const noexcept{
    return std::filesystem::path(this->appDataDir) / std::filesystem::path(this->appDataName);
}

void FileHandler::resetAppData() const noexcept{
    //WORK
}

bool FileHandler::removeAppSetting(const std::string setting_name) const{
    if(!this->isAppDataFile()){
        throw std::runtime_error("App data file not found");
    }
    std::fstream file(this->getAppDataFilePath().c_str());
    std::stringstream file_content;     //stores the data of the file
    std::string line;
    while (std::getline(file, line)){
        std::istringstream iss(line);
        std::string setting, value;
        if (!(iss >> setting >> value)){
            std::cout << "The AppDataFile is not in the right format" << std::endl; // error occured, not in right format
            std::cout << "Do you wanna reset the appData (y/n): ";
            std::string tmp;
            std::cin >> tmp;
            if(tmp == "y"){
                this->resetAppData();
            }
            return false;
        } 
        std::cout << setting << "|" << value << std::endl;  //DEBUGONLY
        if(!(setting == setting_name)){
            //a setting found that is not equal to the setting that should be deleted (we will save it)
            file_content << line << std::endl;
        }
    }
    std::ofstream write_file;
    write_file.open(this->getAppDataFilePath().c_str(), std::ofstream::out | std::ofstream::trunc);
    write_file << file_content.str();       //writes the file again with the read content without the deleted setting
    return true;
}

std::optional<std::string> FileHandler::getAppSetting(const std::string setting_name) const{
    if(!this->isAppDataFile()){
        throw std::runtime_error("App data file not found");
    }
    std::fstream file(this->getAppDataFilePath().c_str());
    std::string line;
    while (std::getline(file, line)){
        std::istringstream iss(line);
        std::string setting, value;
        if (!(iss >> setting >> value)){
            std::cout << "The AppDataFile is not in the right format" << std::endl; // error occured, not in right format
            std::cout << "Do you wanna reset the appData (y/n): ";
            std::string tmp;
            std::cin >> tmp;
            if(tmp == "y"){
                this->resetAppData();
            }
            return{};
        } 
        std::cout << setting << "|" << value << std::endl;  //DEBUGONLY
        if(setting == setting_name){
            //setting is set in the appdata, return the value
            return value;
        }
    }
    return{};
}

bool FileHandler::isAppDataFile() const noexcept{
    return std::filesystem::exists(this->getAppDataFilePath().c_str());
}

bool FileHandler::setAppSetting(const std::string setting_name, const std::string setting_value) const{
    /*
    APP SETTINGS
    filePath -> Path to the current encryption file
    */
    if(!this->isAppDataFile()){
        throw std::runtime_error("App data file not found");
    }
    std::fstream file(this->getAppDataFilePath().c_str());
    std::stringstream file_content;     //stores the data of the file
    std::string line;
    int line_number = -1;
    while (std::getline(file, line)){
        line_number++;
        std::istringstream iss(line);
        std::string setting, value;
        if (!(iss >> setting >> value)){
            std::cout << "The AppDataFile is not in the right format" << std::endl; // error occured, not in right format
            std::cout << "Do you wanna reset the appData (y/n): ";
            std::string tmp;
            std::cin >> tmp;
            if(tmp == "y"){
                this->resetAppData();
            }
            return false;
        } 
        std::cout << setting << "|" << value << std::endl;  //DEBUGONLY
        if(!(setting == setting_name)){
            //setting was not set previously, we have to save it, delete if it is the setting we wanna change
            file_content << line << std::endl;
        }
    }
    //add the new setting to the file content stream
    file_content << setting_name << " " << setting_value << std::endl;

    std::ofstream write_file;
    write_file.open(this->getAppDataFilePath().c_str(), std::ofstream::out | std::ofstream::trunc);
    write_file << file_content.str();       //writes the file again with the read content and the added setting
    return true;
}

bool FileHandler::setEncryptionFilePath(const std::string path) noexcept{
    std::filesystem::path fp{path};
    bool exist = std::filesystem::exists(fp);
    if(exist){
        this->setAppSetting("filePath", path);
        this->encryption_filepath = path;
    }
    return exist;
}

std::string FileHandler::getEncryptionFilePath() const noexcept{
    return this->encryption_filepath;
}

Bytes FileHandler::getFirstBytes(const int num) const{
    if(this->encryption_filepath.empty()){
        throw std::runtime_error("Encrypted filepath is empty");
    }
    std::ifstream file(this->encryption_filepath.c_str()); 
    if(!file){
        throw std::runtime_error("Encrypted file not found in filepath");
    }
    // get length of file:
    file.seekg (0, file.end);
    int length = file.tellg();
    file.seekg (0, file.beg);
    if(length < num){
        //not enough characters to read
        throw std::length_error("File contains to few characters");
    }
    char buf[num];
    file.read(buf, num);
    Bytes ret = Bytes();
    for(char c : buf){
        ret.addByte(static_cast<unsigned char>(c));
    }
    return ret;
}