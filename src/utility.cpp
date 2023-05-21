/*
contains implementations of utility functions from utility.h
*/
#include <vector>
#include "utility.h"

bool endsWith(const std::string &fullString, const std::string &ending) noexcept{
    //checks if a string ends with a given string
    if (fullString.length() >= ending.length()){
        //do the check
        return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
    }else{
        return false;   //if the ending is longer than the actual string, it cannot end with the ending
    }
}

std::vector<unsigned char> LongToCharVec(const unsigned long a) noexcept{
    //transforms a long number into its byte representation and returns the byte vector
    std::vector<unsigned char> ret;
    int i = 0;
    for (i=0; i<8; ++i){
        //a unsigned long has 8 Bytes
        ret.push_back((unsigned char)((((unsigned long) a) >> (56 - (8*i))) & 0xFFu));
    }
    return ret;
}

std::string charVecToString(const std::vector<unsigned char> v) noexcept{
    //transforms the byte vector into a string
    std::string ret;
    //loops over the vector and add each character to the string
    for(size_t i=0; i < v.size(); i++){
        ret += v[i];
    }
    return ret;
}