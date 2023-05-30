/*
contains the implementations of the ChainHashData class
*/
#include <algorithm>
#include "chainhash_data.h"
#include "utility.h"

void ChainHashData::clear() noexcept{
    //clears the data of the object
    this->data_parts.clear();
    this->name_lens.clear();
}

ChainHashData::ChainHashData(Format format): 
    format(format){
    this->clear();  //clears the data from the object
    //gets the name len data to name the data parts and know which length they are
    this->name_lens = format.getNameLenList();
}

bool ChainHashData::isComplete() const noexcept{
    //checks if all data is added by comparing the lengths of the two vectors
    //the name_len vector contains the expected data and data_pars contains the set data
    //no need to check the lengths of each pair, because if new data is added this should be checked there
    return (this->data_parts.size() == this->name_lens.size());
}

bool ChainHashData::isCompletedFormat(Format format) const noexcept{
    //checks if a given format is set
    return (format == this->format && this->isComplete());
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

Bytes ChainHashData::getPart(std::string data_name) const{
    //gets the Bytes data that corresponds to a given data name provided in the format
    for(int i=0; i < this->name_lens.size(); i++){
        //loops through the name_lens list and checks if any name is equal to the given string
        //there has to be at more than the index elements in data parts (in order to get the element)
        if(this->name_lens[i].name == data_name && this->data_parts.size() > i){
            return this->data_parts[i];
        }
    }
    throw std::invalid_argument("Could not find data name in data parts");
}

unsigned int ChainHashData::getLen() const noexcept{
    //gets the length of the set data parts
    unsigned int len = 0;
    for(Bytes part : this->data_parts){
        //loops through all data parts and adds the length of each part
        len += part.getLen();
    }
    return len;
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
    if(current_name_len.len != bytes.getLen() && current_name_len.len != 0){
        //if current len is 0, all lengths are allowed
        //cannot add the given bytes. The length does not match with the set format
        throw std::invalid_argument("tried to add a data part with a not matching length (to the format).");
    }
    this->data_parts.push_back(bytes);  //add the new byte part
    if(this->getLen() > 255){
        //datablock is too long
        std::cout << "Length of datablock: " << this->getLen() << std::endl;
        throw std::logic_error("you added some bytes to the datablock. Now the datablock exceeded the length limit");
    }
}

bool operator==(const ChainHashData &chd1, const ChainHashData &chd2){
    if(chd1.isComplete() && chd2.isComplete()){
        //both objects are complete, compare the data parts
        return (chd1.getDataBlock() == chd2.getDataBlock());
    }
    return false;   //if one of the objects is not complete, they cannot be equal
}
