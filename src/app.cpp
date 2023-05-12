#include "app.h"
#include "utility.h"
#include "pwfunc.h"
#include "dataHeader.h"
#include "settings.h"

bool App::isValidHashMode(std::string mode, bool accept_blank) const noexcept{
    //checks the hash mode (if accept_blank == True, we also acccept an empty string)
    if(accept_blank && mode.empty()){
        //blank is also accepted
        return true;
    }else if(!accept_blank && mode.empty()){
        //blank is not accepted
        return false;
    }
    unsigned char int_mode;
    try{
        int_mode = std::stoi(mode); //transform the mode string to an number
    }catch(std::exception){
        return false;       //the given hash mode string is not convertible into an int
    }
    if(!(0 < std::stoi(mode) && std::stoi(mode) < 256)){
        return false;   //wrong range
    }
    if(HashModes::isModeValid(int_mode)){
        return true;
    }
    return false;   //hash mode has some invalid number (out of range)
}

bool App::isValidChainHashMode(std::string mode, bool accept_blank) const noexcept{
    //checks the chainhash mode (if accept_blank == True, we also acccept an empty string)
    if(accept_blank && mode.empty()){
        //blank is also accepted
        return true;
    }else if(!accept_blank && mode.empty()){
        //blank is not accepted
        return false;
    }
    unsigned char int_mode;
    try{
        int_mode = std::stoi(mode); //transform the mode string to an number
    }catch(std::exception){
        return false;       //the given chainhash mode string is not convertible into an int
    }
    if(!(0 < std::stoi(mode) && std::stoi(mode) < 256)){
        return false;   //wrong range
    }
    if(ChainHashModes::isModeValid(int_mode)){
        return true;
    }
    return false;   //chain hash mode has some invalid number (out of range)
}

bool App::isValidNumber(std::string number, bool accept_blank) const noexcept{
    if(accept_blank && number.empty()){
        //blank is also accepted
        return true;
    }else if(!accept_blank && number.empty()){
        //blank is not accepted
        return false;
    }
    unsigned long res_number;
    try{
        res_number = std::stoul(number); //transform the number string to a number
    }catch(std::exception){
        return false;
    }
    if(MIN_ITERATIONS <= res_number && res_number <= MAX_ITERATIONS){
        return true;
    }
    return false;
}

App::App(){
    this->filePath = this->FH.getEncryptionFilePath();
}

bool App::run(){
    this->printStart();     //get the file location from the user (if not in the app data)
    std::cout << std::endl;
    try{
        this->FH.getFirstBytes(1);      //try to get the first byte of the file
    }catch(std::length_error){
        //file is empty
        //construct a basic file header with a password from the user
        std::cout << "It seems that the encrypted file is empty. Let`s set up this file" << std::endl;
        unsigned char hash_mode = this->askForHashMode();
        //std::cout << "Hash Mode " << +hash_mode << " selected: " << std::endl << std::endl;
        std::cout << HashModes::getInfo(hash_mode) << std::endl << std::endl;
        unsigned char chainhash_mode1 = this->askForChainHashMode();
        //WORK
        unsigned long pass_val_iters = this->askForPasswdIters();
        std::cout << pass_val_iters << " iterations selected" << std::endl << std::endl;
        std::string pw = this->askForPasswd();
        return false; //DEBUGONLY

    }
    return false; //DEBUGONLY
    unsigned char mode = this->FH.getFirstBytes(1).getBytes()[0];      //get the mode for the encrypted file
    DataHeader DH(mode);
    Bytes header = this->FH.getFirstBytes(DH.getHeaderLength());  
    DH.setHeaderBytes(header);
    std::string pw = this->askForPasswd();
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
        while(filename.empty()){
            std::cout << "Please enter a valid filename: ";
            getline(std::cin, filename);
        }
        if(!endsWith(filename, FileHandler::extension))filename += FileHandler::extension;
        std::ifstream src(filename.c_str(), std::ifstream::in);
        if(!src){
            std::cout << "Provided filename is not yet created" << std::endl;
            std::ofstream new_file(filename.c_str());
            std::cout << "New encryption file: " << filename << " created" << std::endl;
        }
        this->filePath = std::filesystem::current_path() / std::filesystem::path(filename);
        std::cout << std::endl;
    }
    if(!this->FH.setEncryptionFilePath(this->filePath)){  //save the new file path in the appdata
        //newly created file not found
        throw std::logic_error("File not found");
    }
    std::cout << "The current encryption file location is: " << this->filePath << std::endl;
    
}

std::string App::askForPasswd() const noexcept{
    std::string pw;
    while(true){
        std::cout << "Please enter the password for this file (if it is a new file, this password will be set): ";
        getline(std::cin, pw);
        std::cout << std::endl;
        if(!PwFunc::isPasswordValid(pw)){
            std::cout << "Your password contains some illegal chars or is not long enough" << std::endl;
            //WORK Feedback the exact error to the user //ISSUE
            continue;
        }
        break;
    }
    return pw;
}

unsigned char App::askForHashMode() const noexcept{
    //this function asks the user for the hash mode (blank means standard)
    std::string hash_mode_inp;
    unsigned char hash_mode;
    do{
        std::cout << "Enter the hash mode (1-" << +MAX_HASHMODE_NUMBER <<")(leave blank to set the standard [" << +STANDARD_HASHMODE <<"]): ";
        hash_mode_inp = "";
        getline(std::cin, hash_mode_inp);
    }while (!this->isValidHashMode(hash_mode_inp, true));
    
    if(hash_mode_inp.empty()){
        //set the standard mode
        hash_mode = STANDARD_HASHMODE;
    }else{
        hash_mode = std::stoi(hash_mode_inp); //set the user given mode
    }
    return hash_mode;
}

unsigned char App::askForChainHashMode() const noexcept{
    //this function asks the user for the chain hash mode (blank means standard)
    std::string chainhash_mode_inp;
    unsigned char chainhash_mode;
    do{
        std::cout << "Enter the chainhash mode (1-" << +MAX_CHAINHASHMODE_NUMBER <<")(leave blank to set the standard [" << +STANDARD_CHAINHASHMODE <<"]): ";
        chainhash_mode_inp = "";
        getline(std::cin, chainhash_mode_inp);
    }while (!this->isValidChainHashMode(chainhash_mode_inp, true));
    
    if(chainhash_mode_inp.empty()){
        //set the standard mode
        chainhash_mode = STANDARD_CHAINHASHMODE;
    }else{
        chainhash_mode = std::stoi(chainhash_mode_inp); //set the user given mode
    }
    return chainhash_mode;
}

unsigned long App::askForPasswdIters() const noexcept{
    //asks the user for a password and also makes sure its not invalid (still can be wrong obv)
    std::string iter_inp;
    unsigned long iter;
    do{
        std::cout << "How many iterations should be used to validate your password (leave blank to set the standard [" << STANDARD_PASS_VAL_ITERATIONS << "]): ";
        iter_inp = "";
        getline(std::cin, iter_inp);
    }while (!this->isValidNumber(iter_inp, true));

    if(iter_inp.empty()){
        //set the standard iterations
        iter = STANDARD_PASS_VAL_ITERATIONS;
    }else{
        iter = std::stoul(iter_inp); //set the user given iterations
    }
    return iter;

}
