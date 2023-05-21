/*
contains the implementations of the ChainHashData class
*/
#include "chainhash_data.h"
#include "utility.h"

void ChainHashData::clear() noexcept{
    //clears the data of the object
    this->data_parts.clear();
    this->name_lens.clear();
}

ChainHashData::ChainHashData(std::string format){
    //format infos in chainhash_modes.md
    //format <length in bytes>B <name>
    this->clear();  //clears the data from the object
    if(format.empty()){
        //got an empty format, user wants an empty datablock
        return;
    }
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
            //getting the size of the part
            if(!endsWith(substring, "B")){
                //size does not end with B (wrong format)
                throw std::invalid_argument("format is not valid. Contains no B on the end of size part.");
            }
            substring.erase(substring.end());   //delete the B
            int size_int = 0;
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
            //valid size
        }else{
            if(data_len == 0){
                throw std::invalid_argument("format is not valid. Got data name with 0 bytes length.");
            }
            this->name_lens.push_back(NameLen(substring, data_len));
        }
        format.erase(0, ind + 1);    //erase the substring + the separator " ", which's length is 1
    }
    if(!(0 < this->name_lens.size() < 256)){
        //the algorithm did not add anything to the list, but the user entered a non empty format string
        //or the user inputs more than 256 datasets
        std::cout << "Format has to contain between 0 and 256 entries exclusive";
        std::cout << "or the format string has to be empty for 0 entries" << std::endl;
        throw std::invalid_argument("format is not valid. Got invalid entry number from a non empty format");
    }
}

bool ChainHashData::isComplete() const noexcept{
    //checks if all data is added by comparing the lengths of the two vectors
    //the name_len vector contains the expected data and data_pars contains the set data
    //no need to check the lengths of each pair, because if new data is added this should be checked there
    return (this->data_parts.size() == this->name_lens.size());
}

unsigned char ChainHashData::getPartsNumber() const noexcept{
    //returns the number of currently set data parts
    return this->data_parts.size();
}

Bytes ChainHashData::getDataBlock() const{
    //gets the datablock by concatinating the data parts
    if(!this->isComplete()){
        //throws if the block is not completed yet
        throw std::logic_error("trying to get datablock from a non complete ChainHashData object.");
    }
    Bytes ret;
    for(Bytes part : this->data_parts){
        //add each part to the Bytes object
        ret.addBytes(part);
    }
    return ret;
}

void ChainHashData::addBytes(Bytes bytes){
    //adds a new part to the data parts vector
    if(this->isComplete()){
        //cannot proceed, the datablock is complete. No new data can be added
        throw std::logic_error("Datablock is completed. No new data can be added");
    }
    unsigned char current_parts = this->getPartsNumber();   //gets the current parts
    //the number of current parts is also the index for the next part (indices start from 0)
    NameLen current_name_len = this->name_lens[current_parts];  //gets the format information
    if(current_name_len.len != bytes.getLen()){
        //cannot add the given bytes. The length does not match with the set format
        throw std::invalid_argument("tried to add a data part with a not matching length (to the format).");
    }
    this->data_parts.push_back(bytes);  //add the new byte part
}
