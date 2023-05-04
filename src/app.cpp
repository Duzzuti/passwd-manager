#include "app.h"
#include "utility.h"

App::App(){
    this->filePath = this->FH.getFilePath();
}

bool App::run(){
    this->printStart();
    return true;
}

void App::printStart(){
    std::cout << "Welcome to the local encryption system" << std::endl;
    if(this->filePath.empty()){
        std::cout << "The current encryption file location is: " << "not set" << std::endl;
        std::cout << "The new file location will be set to the current directory." << std::endl;
        std::cout << "Please enter the name of the encryption file (if it does not exist it will be created in the current location): ";
        std::string filename{};
        getline(std::cin, filename);
        if(!endsWith(filename, FileHandler::extension))filename += FileHandler::extension;
        std::ifstream src(filename.c_str(), std::ifstream::in);
        if(!src){
            std::cout << "Provided filename is not yet created" << std::endl;
            std::ofstream new_file(filename.c_str());
            std::cout << "New encryption file: " << filename << " created" << std::endl;
        }
        this->filePath = std::filesystem::current_path() / std::filesystem::path(filename);
    }
    if(!this->FH.setFilePath(this->filePath)){  //save the new file path in the appdata
        //newly created file not found
        throw std::logic_error("File not found");
    }
    std::cout << "The current encryption file location is: " << this->filePath << std::endl;
    
}
