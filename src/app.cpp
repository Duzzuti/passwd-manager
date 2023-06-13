/*
the implementation of the App class is done in this file
*/
#include "app.h"

#include <memory>

#include "chainhash_data.h"
#include "dataheader.h"
#include "file_data.h"
#include "file_modes.h"
#include "pwfunc.h"
#include "settings.h"
#include "utility.h"

bool App::isValidChar(const std::string mode) noexcept {
    // checks if the mode is a valid unsigned char
    unsigned char mode_char;
    try {
        mode_char = std::stoi(mode);  // transform the mode string to an number
    } catch (std::exception) {
        return false;  // the given mode string is not convertible into an char
    }
    // checks if the entered mode is an unisgned char (if the user enteres 257, mode_char will be 2 and it looks valid)
    if (!(0 <= std::stoi(mode) && std::stoi(mode) < 256)) {
        return false;  // not an unsigned char
    }
    return true;
}

bool App::isValidFileMode(const std::string mode, const bool accept_blank) noexcept {
    // checks the file mode (if accept_blank == True, we also acccept an empty string)
    if (mode.empty()) {
        // if the mode was empty return if blank is accepted
        return accept_blank;
    }
    if (!App::isValidChar(mode)) {
        return false;  // input mode was not a valid char
    }
    // checks the actual range of the file modes
    if (FileModes::isModeValid(FModes(std::stoi(mode)))) {
        return true;
    }
    return false;  // file mode has some invalid number (out of range)
}

bool App::isValidHashMode(const std::string mode, const bool accept_blank) noexcept {
    // checks the hash mode (if accept_blank == True, we also acccept an empty string)
    if (mode.empty()) {
        // if the mode was empty return if blank is accepted
        return accept_blank;
    }
    if (!App::isValidChar(mode)) {
        return false;  // input mode was not a valid char
    }
    // checks the actual range of the hash mode
    if (HashModes::isModeValid(HModes(std::stoi(mode)))) {
        return true;
    }
    return false;  // hash mode has some invalid number (out of range)
}

bool App::isValidChainHashMode(const std::string mode, const bool accept_blank) noexcept {
    // checks the chainhash mode (if accept_blank == True, we also acccept an empty string)
    if (mode.empty()) {
        // if the mode was empty return if blank is accepted
        return accept_blank;
    }
    if (!App::isValidChar(mode)) {
        return false;  // input mode was not a valid char
    }
    // checks the actual range of the chain hash modes
    if (ChainHashModes::isModeValid(CHModes(std::stoi(mode)))) {
        return true;
    }
    return false;  // chain hash mode has some invalid number (out of range)
}

bool App::isValidNumber(const std::string number, const bool accept_blank, const u_int64_t lower_bound, const u_int64_t upper_bound) noexcept {
    if (number.empty()) {
        // if the number string was empty return if blank is accepted
        return accept_blank;
    }
    u_int64_t long_number;
    try {
        long_number = std::stoul(number);  // transform the number string to a number
    } catch (std::exception) {
        return false;  // number string could not be transformed into an u_int64_t
    }
    // checks the bounds of the number
    if (lower_bound <= long_number && long_number <= upper_bound) {
        return true;
    }
    return false;  // given number is not in the valid number bounds
}

App::App() {
    // main constructor is setting up some details about the application
    // gets the filepath to the current selected encryption file
    // if there is no file set we will get an empty string
    this->filePath = this->FH.getEncryptionFilePath();
}

bool App::run() {
    // main application function
    // the whole app lives here
    this->printStart();  // get the file location from the user (if not set in the app data)
    std::cout << std::endl;
    Bytes header;
    std::unique_ptr<FileData*> filedata;
    try {
        this->FH.getFirstBytes(1);  // try to get the first byte of the file
    } catch (std::length_error) {
        // file is empty
        // construct a basic file header with a password from the user
        std::cout << "It seems that the encrypted file is empty. Let`s set this file up" << std::endl;
        header = this->askForHeader();  // gets an header by asking the user several questions and the password
    }
    // file was not empty TODO
    return true;
}

void App::printStart() {
    // prints the welcome text and gets the location of the encryption file if its not set
    std::cout << "Welcome to the local encryption system" << std::endl;
    if (this->filePath.empty()) {
        // encryption file is not set yet
        std::cout << "The current encryption file location is: "
                  << "not set" << std::endl;
        std::cout << "The new file location will be set to the current directory." << std::endl;
        std::cout << "Please enter the name of the encryption file (if it does not exist it will be created in the current location): ";
        std::string filename{};
        getline(std::cin, filename);  // gets the name of the encryption file that will be created in the current dir
        while (filename.empty()) {
            std::cout << "Please enter a valid filename: ";
            getline(std::cin, filename);
        }
        if (!endsWith(filename, FileHandler::extension)) filename += FileHandler::extension;  // add the extension if its missing
        std::ifstream src(filename.c_str(), std::ifstream::in);                               // create the input file stream for that file
        if (!src) {
            // if the file does not exist
            std::cout << "Provided filename is not yet created" << std::endl;
            std::ofstream new_file(filename.c_str());  // creates a new empty file with the given name
            std::cout << "New encryption file: " << filename << " created" << std::endl;
        }
        // sets the encryption file to the new created path
        this->filePath = std::filesystem::current_path() / std::filesystem::path(filename);
        std::cout << std::endl;
    }
    // valid file exists
    if (!this->FH.setEncryptionFilePath(this->filePath)) {  // save the new file path in the appdata
        // newly created file not found
        throw std::logic_error("File not found");
    }
    // location of the file gets printed
    std::cout << "The current encryption file location is: " << this->filePath << std::endl;
}

std::string App::askForPasswd() const noexcept {
    // asks the user for a password
    std::string pw;
    while (true) {
        std::cout << "Please enter the password for this file (if it is a new file, this password will be set): ";
        getline(std::cin, pw);
        std::cout << std::endl;
        ErrorStruct err_struct = PwFunc::isPasswordValid(pw);  // checks whether the password is valid
        if (!err_struct.success == SUCCESS) {
            // no success, password not valid
            std::cout << getErrorMessage(err_struct) << std::endl;  // prints the reason why the password is not valid
            continue;
        }
        break;
    }
    return pw;  // returns the gotten password
}

FModes App::askForFileMode() const noexcept {
    // this function asks the user for the file mode (blank means standard)
    std::string file_mode_inp;
    unsigned char file_mode;
    do {
        std::cout << "Enter the file mode (1-" << +MAX_FILEMODE_NUMBER << ")(leave blank to set the standard [" << +STANDARD_FILEMODE << "]): ";
        file_mode_inp = "";
        getline(std::cin, file_mode_inp);                   // gets the input from the user
    } while (!this->isValidFileMode(file_mode_inp, true));  // checks if the input is valid (accept blank)

    if (file_mode_inp.empty()) {
        // set the standard mode because the user did not enter anything
        file_mode = STANDARD_FILEMODE;
    } else {
        file_mode = std::stoi(file_mode_inp);  // set the user given mode
    }
    return FModes(file_mode);  // returns the enum type of the file mode
}

HModes App::askForHashMode() const noexcept {
    // this function asks the user for the hash mode (blank means standard)
    std::string hash_mode_inp;
    unsigned char hash_mode;
    do {
        std::cout << "Enter the hash mode (1-" << +MAX_HASHMODE_NUMBER << ")(leave blank to set the standard [" << +STANDARD_HASHMODE << "]): ";
        hash_mode_inp = "";
        getline(std::cin, hash_mode_inp);                   // gets the input from the user
    } while (!this->isValidHashMode(hash_mode_inp, true));  // checks if the input is valid (accept blank)

    if (hash_mode_inp.empty()) {
        // set the standard mode because the user did not enter anything
        hash_mode = STANDARD_HASHMODE;
    } else {
        hash_mode = std::stoi(hash_mode_inp);  // set the user given mode
    }
    return HModes(hash_mode);  // returns the enum type of the hash mode
}

CHModes App::askForChainHashMode() const noexcept {
    // this function asks the user for the chain hash mode (blank means standard)
    std::string chainhash_mode_inp;
    unsigned char chainhash_mode;
    do {
        std::cout << "Enter the chainhash mode (1-" << +MAX_CHAINHASHMODE_NUMBER << ")(leave blank to set the standard [" << +STANDARD_CHAINHASHMODE << "]): ";
        chainhash_mode_inp = "";
        getline(std::cin, chainhash_mode_inp);                        // gets the input from the user
    } while (!this->isValidChainHashMode(chainhash_mode_inp, true));  // checks if the input is valid (accept blank)

    if (chainhash_mode_inp.empty()) {
        // set the standard mode because the user did not enter anything
        chainhash_mode = STANDARD_CHAINHASHMODE;
    } else {
        chainhash_mode = std::stoi(chainhash_mode_inp);  // set the user given mode
    }
    return CHModes(chainhash_mode);
}

u_int64_t App::askForIters(const std::string msg) const noexcept {
    // asks the user for the number of iterations that should be applied when deriving the passwordhash
    std::string iter_inp;
    u_int64_t iter;
    do {
        std::cout << msg << " (leave blank to set the standard [" << STANDARD_ITERATIONS << "]): ";
        iter_inp = "";
        getline(std::cin, iter_inp);                                                 // gets the input from the user
    } while (!this->isValidNumber(iter_inp, true, MIN_ITERATIONS, MAX_ITERATIONS));  // checks if the input is a valid number (accept blank)

    if (iter_inp.empty()) {
        // set the standard iterations because the user did not enter anything
        iter = STANDARD_ITERATIONS;
    } else {
        iter = std::stoul(iter_inp);  // set the user given iterations
    }
    return iter;
}

Bytes App::askForHeader() const {
    // returns a valid header with the user preferences
    FModes file_data_mode = this->askForFileMode();  // ask for file mode
    HModes hash_mode = this->askForHashMode();       // ask for hash mode
    // ask for chainhash1 data, which is used to get the passwordhash from the password
    std::cout << std::endl;
    std::cout << "In order to get an hash derived from the enterd password, we need to perform a chainhash" << std::endl;
    std::cout << "Please enter the preferences for this chainhash:" << std::endl << std::endl;
    CHModes chainhash_mode1 = this->askForChainHashMode();
    std::cout << std::endl;  // chainhash1 mode
    u_int64_t chainhash_iters1 = this->askForIters("How many iterations should be used to derive a hash from your password");
    std::cout << std::endl;
    // for every chainhash mode there are different things we need for the header. Thats why we ask this data in the ChainHashModes class
    ChainHashData datablock1 = ChainHashModes::askForData(chainhash_mode1);                              // gets the datablock for chainhash1
    ErrorStruct err1 = ChainHashModes::isChainHashValid(chainhash_mode1, chainhash_iters1, datablock1);  // checks whether the chainhash1 is valid
    if (!err1.success == SUCCESS) {
        // checks whether the datablock1 has a valid format
        throw std::length_error(getErrorMessage(err1));
    }
    unsigned char datablock_len1 = datablock1.getLen();  // gets the length in Bytes from the datablock1
    // ask for chainhash2 data, which is used to get the passwordhashhash from the passwordhash (for validating the passwordhash)
    std::cout << std::endl;
    std::cout << "In order to verify the entered password, we need to perform an other chainhash" << std::endl;
    std::cout << "Please enter the preferences for this chainhash as well" << std::endl << std::endl;
    CHModes chainhash_mode2 = this->askForChainHashMode();
    std::cout << std::endl;  // chainhash2 mode
    u_int64_t chainhash_iters2 = this->askForIters("How many iterations should be used to validate the password");
    std::cout << std::endl;
    // for every chainhash mode there are different things we need for the header. Thats why we ask this data in the ChainHashModes class
    ChainHashData datablock2 = ChainHashModes::askForData(chainhash_mode2);                              // gets the datablock for chainhash2
    ErrorStruct err2 = ChainHashModes::isChainHashValid(chainhash_mode2, chainhash_iters2, datablock2);  // checks whether the chainhash2 is valid
    if (!err2.success == SUCCESS) {
        // checks whether the datablock2 has a valid format
        throw std::length_error(getErrorMessage(err2));
    }
    unsigned char datablock_len2 = datablock2.getLen();  // gets the length in Bytes from the datablock2

    // print a summary
    std::cout << std::endl;
    std::cout << "Following preferences will be set for this file: " << std::endl;
    std::cout << "[FILE DATA MODE] " << FileModes::getInfo(file_data_mode) << std::endl;
    std::cout << "[HASH FUNCTION] " << HashModes::getInfo(hash_mode) << std::endl << std::endl;
    std::cout << "[CHAINHASH1] (to derive the hash from the password):" << std::endl;
    std::cout << "\t[MODE] " << ChainHashModes::getInfo(chainhash_mode1) << std::endl;
    std::cout << "\t[ITERATIONS] " << chainhash_iters1 << std::endl;
    std::cout << "\t[DATABLOCKLEN] " << +datablock_len1 << " Bytes" << std::endl;
    std::cout << "\t[DATABLOCK] " << toHex(datablock1.getDataBlock()) << std::endl << std::endl;

    std::cout << "[CHAINHASH2] (to validate the hash from the password):" << std::endl;
    std::cout << "\t[MODE] " << ChainHashModes::getInfo(chainhash_mode2) << std::endl;
    std::cout << "\t[ITERATIONS] " << chainhash_iters2 << std::endl;
    std::cout << "\t[DATABLOCKLEN] " << +datablock_len2 << " Bytes" << std::endl;
    std::cout << "\t[DATABLOCK] " << toHex(datablock2.getDataBlock()) << std::endl << std::endl;

    std::string pw = this->askForPasswd();  // gets the password from the user that should be used to encrypt the file

    std::cout << "Let's generate the password validator" << std::endl;
    std::cout << "Please note that this is the minimum time you need to decrypt you file. ";
    std::cout << "If its really slow you can set up the file again but with a faster chainhash or fewer iterations to match your hardware. ";
    std::cout << "Keep in mind that you need a stronger password if the decrypt time is shorter (its shorter too for an attacker, who can faster bruteforce your password).";
    // WORK time measurement //ISSUE
    std::cout << std::endl << "Generating password hash..." << std::endl;
    Hash* hash = HashModes::getHash(hash_mode);
    Bytes pwhash = ChainHashModes::performChainHash(chainhash_mode1, chainhash_iters1, datablock1, hash, pw);  // calculate passwordhash
    std::cout << "Password hash generated. Generating password validator..." << std::endl;
    Bytes pwval = ChainHashModes::performChainHash(chainhash_mode2, chainhash_iters2, datablock2, hash, pwhash);  // calculate passwordhashhash
    std::cout << "Password validator generated." << std::endl;
    delete hash;
    std::cout << "PW HASH: " << toHex(pwhash) << std::endl;      // DEBUGONLY
    std::cout << "PW VALIDATOR: " << toHex(pwval) << std::endl;  // DEBUGONLY

    // create the dataheader with the data got from the user
    DataHeader DH = DataHeader(hash_mode);
    DH.setFileDataMode(file_data_mode);
    DH.setChainHash1(chainhash_mode1, chainhash_iters1, datablock_len1, datablock1);
    DH.setChainHash2(chainhash_mode2, chainhash_iters2, datablock_len2, datablock2);
    DH.setValidPasswordHashBytes(pwval);
    DH.calcHeaderBytes(pwhash);                                              // use the pwhash to encrypt the random generated salt
    Bytes header = DH.getHeaderBytes();                                      // gets the header
    std::cout << "FULL HEADER:" << std::endl << toHex(header) << std::endl;  // DEBUGONLY
    return header;
}
