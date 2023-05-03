#include "app.h"
#include "filehandler.h"

App::App(){
    FileHandler FH;
    this->filePath = FH.getFilePath();
}

bool App::run(){
    this->printStart();
    return true;
}

void App::printStart() const noexcept{
    std::cout << "Welcome to the local encryption system" << std::endl;
    if(this->filePath.empty()){
        std::cout << "The current encryption file location is: " << "not set" << std::endl;
        std::cout << "Please set the encryption file location. Enter the path to the encryption file (if it does not exist it will be created)" << std::endl;
        std::cout << "You can also input a filename and it gets created in the current dir: ";
        std::string user_inp;
        std::cin >> user_inp;
        std::filesystem::path user_path(user_inp);
    }else{
        std::cout << "The current encryption file location is: " << this->filePath << std::endl;
    }
    
}
