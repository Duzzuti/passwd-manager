#include <vector>
#include "utility.h"

bool endsWith(std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}
std::vector<unsigned char> LongToCharVec(unsigned long a){
    std::vector<unsigned char> ret;
    int i = 0;
    for (i=0; i<8; ++i){
        ret.push_back((unsigned char)((((unsigned long) a) >> (56 - (8*i))) & 0xFFu));
    }
    return ret;
}

std::string charVecToString(std::vector<unsigned char> v){
    std::string ret;
    for(size_t i=0; i < v.size(); i++){
        ret += v[i];
    }
    return ret;
}