#include <random>
#include <iostream>
#include "bytes.h"

Bytes::Bytes(){
    this->bytes.clear();
}

Bytes::Bytes(const int len){
    this->bytes.clear();
    std::random_device rd;
    std::uniform_int_distribution<int> rand(0, 255);
    for(int i=0; i<len; i++){
        this->bytes.push_back(rand(rd));
    }
}

void Bytes::print() const noexcept{
    for(unsigned char byte : this->bytes){
        std::cout << toHex(byte);
    }
    std::cout << std::endl;
}

void Bytes::setBytes(std::vector<unsigned char> bytes) noexcept{
    this->bytes = bytes;
}

std::vector<unsigned char> Bytes::getBytes() const noexcept{
    return this->bytes;
}

int Bytes::getLen() const noexcept{
    return this->bytes.size();
}

void Bytes::addByte(const unsigned char byte) noexcept{
    this->bytes.push_back(byte);
}

std::optional<Bytes> Bytes::popFirstBytes(const int num){
    std::optional<Bytes> firstbytes = this->getFirstBytes(num);
    if(firstbytes.has_value()){
        auto first = this->bytes.begin() + num;
        auto last = this->bytes.end();
        this->bytes = std::vector<unsigned char>(first, last);
    }
    return firstbytes;
}

std::optional<Bytes> Bytes::getFirstBytes(const int num) const{
    if(num > this->bytes.size()){
        return {};    
    }
    Bytes ret = Bytes();
    for(int i=0; i<num; i++){
        ret.addByte(this->bytes[i]);
    }
    return ret;
}

std::string toHex(const unsigned char byte) noexcept{
    char hex[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    std::string ret;
    ret += hex[(byte/16) % 16];
    ret += hex[byte % 16];
    return ret;
}
