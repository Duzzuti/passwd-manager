#include "app.h"
#include "utility.h"
#include "pwfunc.h"
#include "dataHeader.h"
#include "settings.h"

bool App::isValidHashMode(std::string mode, bool accept_blank) noexcept{
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

bool App::isValidChainHashMode(std::string mode, bool accept_blank) noexcept{
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

bool App::isValidNumber(std::string number, bool accept_blank, unsigned long lower_bound, unsigned long upper_bound) noexcept{
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
    if(lower_bound <= res_number && res_number <= upper_bound){
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
        std::cout << std::endl;
        std::cout << "In order to get an hash derived from the enterd password, we need to perform a chainhash" << std::endl;
        std::cout << "Please enter the preferences for this chainhash:" << std::endl << std::endl;
        unsigned char chainhash_mode1 = this->askForChainHashMode();std::cout << std::endl;
        unsigned long chainhash_iters1 = this->askForIters("How many iterations should be used to derive a hash from your password");std::cout << std::endl;
        Bytes datablock1 = ChainHashModes::askForData(chainhash_mode1);
        if(datablock1.getLen() > 255 || !ChainHashModes::isChainHashValid(chainhash_mode1, chainhash_iters1, datablock1)){
            throw std::length_error("Chainhash1 data was invalid");
        }
        unsigned char datablock_len1 = datablock1.getLen();
        std::cout << std::endl;
        std::cout << "In order to verify the entered password, we need to perform an other chainhash" << std::endl;
        std::cout << "Please enter the preferences for this chainhash as well" << std::endl << std::endl;
        unsigned char chainhash_mode2 = this->askForChainHashMode();std::cout << std::endl;
        unsigned long chainhash_iters2 = this->askForIters("How many iterations should be used to validate the password");std::cout << std::endl;
        Bytes datablock2 = ChainHashModes::askForData(chainhash_mode2);std::cout << std::endl;
        if(datablock2.getLen() > 255 || !ChainHashModes::isChainHashValid(chainhash_mode2, chainhash_iters2, datablock2)){
            throw std::length_error("Chainhash2 data was invalid");
        }
        unsigned char datablock_len2 = datablock2.getLen();

        //print a summary
        std::cout << std::endl;
        std::cout << "Following preferences will be set for this file: " << std::endl;
        std::cout << "[HASH FUNCTION] " << HashModes::getInfo(hash_mode) << std::endl << std::endl;
        std::cout << "[CHAINHASH1] (to derive the hash from the password):" << std::endl;
        std::cout << "\t[MODE] " << ChainHashModes::getInfo(chainhash_mode1) << std::endl;
        std::cout << "\t[ITERATIONS] " << chainhash_iters1 << std::endl;
        std::cout << "\t[DATABLOCKLEN] " << +datablock_len1 << " Bytes" << std::endl;
        std::cout << "\t[DATABLOCK] " << toHex(datablock1) << std::endl << std::endl;

        std::cout << "[CHAINHASH2] (to validate the hash from the password):" << std::endl;
        std::cout << "\t[MODE] " << ChainHashModes::getInfo(chainhash_mode2) << std::endl;
        std::cout << "\t[ITERATIONS] " << chainhash_iters2 << std::endl;
        std::cout << "\t[DATABLOCKLEN] " << +datablock_len2 << " Bytes" << std::endl;
        std::cout << "\t[DATABLOCK] " << toHex(datablock2) << std::endl << std::endl;

        std::string pw = this->askForPasswd();

        std::cout << "Let's generate the password validator" << std::endl;
        std::cout << "Please note that this is the minimum time you need to decrypt you file. ";
        std::cout << "If its really slow you can set up the file again but with a faster chainhash or fewer iterations to match your hardware. ";
        std::cout << "Keep in mind that you need a stronger password if the decrypt time is shorter (its shorter too for an attacker, who can faster bruteforce your password).";
        //WORK time measurement //ISSUE
        std::cout << std::endl << "Generating password hash..." << std::endl;
        Hash* hash = HashModes::getHash(hash_mode);
        Bytes pwhash = ChainHashModes::performChainHash(chainhash_mode1, chainhash_iters1, datablock1, hash, pw);
        std::cout << "Password hash generated. Generating password validator..." << std::endl;
        Bytes pwval = ChainHashModes::performChainHash(chainhash_mode2, chainhash_iters2, datablock2, hash, pwhash);
        std::cout << "Password validator generated." << std::endl; 
        delete hash;
        std::cout << "PW HASH: " << toHex(pwhash) << std::endl;         //DEBUGONLY
        std::cout << "PW VALIDATOR: " << toHex(pwval) << std::endl;     //DEBUGONLY
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

unsigned long App::askForIters(std::string msg) const noexcept{
    //asks the user for the number of iterations that should be applied when deriving the passwordhash
    std::string iter_inp;
    unsigned long iter;
    do{
        std::cout << msg << " (leave blank to set the standard [" << STANDARD_ITERATIONS << "]): ";
        iter_inp = "";
        getline(std::cin, iter_inp);
    }while (!this->isValidNumber(iter_inp, true, MIN_ITERATIONS, MAX_ITERATIONS));

    if(iter_inp.empty()){
        //set the standard iterations
        iter = STANDARD_ITERATIONS;
    }else{
        iter = std::stoul(iter_inp); //set the user given iterations
    }
    return iter;

}
