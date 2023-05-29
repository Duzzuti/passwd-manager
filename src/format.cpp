/*
contains the implementations of the format class
*/

#include <algorithm>

#include "format.h"
#include "chainhash_modes.h"
#include "utility.h"

Format::Format(const CHModes chainhash_mode){
    //baisc constructor sets up the format string for the given chainhash mode
    if(!ChainHashModes::isModeValid(chainhash_mode)){
        //given chainhash mode is not valid
        throw std::invalid_argument("given chainhash mode is not valid");
    }
    //sets up the correct format string for that chainhash mode
    switch(chainhash_mode){
        case CHAINHASH_NORMAL:
            this->format = "";
            break;
        case CHAINHASH_CONSTANT_SALT:
            this->format = "*B S";
            break;
        case CHAINHASH_COUNT_SALT:
            this->format = "8B SN";
            break;
        case CHAINHASH_CONSTANT_COUNT_SALT:
            this->format = "8B SN *B S";
            break;
        case CHAINHASH_QUADRATIC:
            this->format = "8B SN 8B A 8B B 8B C";
            break;
        default:    //invalid chainhash mode, should not throw
            throw std::logic_error("invalid chainhash mode");
    }
    this->chainhash_mode = chainhash_mode;
}

CHModes Format::getChainMode() const noexcept{
    return this->chainhash_mode;
}

std::vector<NameLen> Format::getNameLenList() const{
    //gets a List with NameLens structs
    //every entry describes one data part with its name and its length in bytes
    //for docs view chainhash_modes.md
    //format infos in chainhash_modes.md
    //format <length in bytes>B <name>
    std::string format = this->format;
    if(format.empty()){
        //got an empty format, user wants an empty datablock
        return std::vector<NameLen>();
    }
    //saves all data names, to make sure that there are no duplicates in the format
    std::vector<std::string> data_names;
    std::vector<NameLen> name_lens;
    int accumulated_size = 0;   //stores the complete size of the block
    //there can only be one star element (element with variable length) and it has to be the last one
    //keeps track if we have reached a start element
    bool star = false;          
    size_t ind = 0;
    //data_name is true if the part that is looked on is a data_name.
    //if it is false, that means we are looking on the size part
    bool data_name = false;
    //stores the length of the next data (because we get the length before the name)
    unsigned char data_len = 0;
    //loops over every format substring seperated by " "
    while ((ind = format.find(" ")) != std::string::npos) {
        std::string substring = format.substr(0, ind);  //gets the substring with the given index
        if(substring.empty()){
            throw std::invalid_argument("format is not valid. Got some empty substring");
        }
        if(!data_name){
            if(star){
                //a star element has to be the last one
                throw std::invalid_argument("format is not valid. Star element is not the last element");
            }
            //getting the size of the part
            if(!endsWith(substring, "B")){
                //size does not end with B (wrong format)
                throw std::invalid_argument("format is not valid. Contains no B on the end of size part.");
            }
            substring.pop_back();   //delete the B
            int size_int = 0;
            //if the format is *B, that means that any length (greater than zero is accepted)
            //we mark this with a length of zero
            if(substring != "*"){
                try{
                    size_int = std::stoi(substring);    //transform the string to an int
                }catch(std::exception){
                    //could not convert string to int
                    throw std::invalid_argument("format is not valid. Size could not be converted to int");
                }
                if(!(0 < size_int < 256)){
                    //provided size is not valid
                    throw std::invalid_argument("format is not valid. Size has to be between 0 and 256 exclusive.");
                }
            }else{
                star = true;
            }
            //valid size
            data_len = size_int;
            accumulated_size += size_int;
            if(accumulated_size > 255) {
                //datablock gets bigger than allowed
                throw std::invalid_argument("format is not valid. Accumulated size gets greater than 255 bytes");
            }
        }else{
            if(std::find(data_names.begin(), data_names.end(), substring) != data_names.end()){
                //found the current data name already in the list
                throw std::invalid_argument("format is not valid. Found multiple parts with the same name");
            }
            data_names.push_back(substring);
            name_lens.push_back(NameLen(substring, data_len));
        }
        format.erase(0, ind + 1);    //erase the substring + the separator " ", which's length is 1
        data_name = !data_name;     //flip the bool, data names and sizes are alternating
    }
    if(!(0 < name_lens.size() < 256)){
        //the algorithm did not add anything to the list, but the user entered a non empty format string
        //or the user inputs more than 256 datasets
        std::cout << "Format has to contain between 0 and 256 entries exclusive";
        std::cout << "or the format string has to be empty for 0 entries" << std::endl;
        throw std::invalid_argument("format is not valid. Got invalid entry number from a non empty format");
    }
    return name_lens;
}

bool operator==(const Format &f1, const Format &f2){
    //compare two formats by their format strings
    return f1.format == f2.format;
}
