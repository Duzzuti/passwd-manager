#include <random>
#include "bytes.h"
#include "rng.h"

Bytes::Bytes(){
    this->clear();
}

Bytes::Bytes(const int len){
    if(len < 0){
        throw std::range_error("The provided len is negative");
    }
    this->setBytes(RNG().get_random_bytes(len));
}

void Bytes::print() const noexcept{
    std::cout << toHex(*this);
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

void Bytes::addBytes(const Bytes b1) noexcept{
    for(unsigned char byte : b1.getBytes()){
        this->addByte(byte);
    }
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
    if(num < 0){
        throw std::range_error("The provided len is negative");
    }
    if(num > this->bytes.size()){
        return {};    
    }
    Bytes ret = Bytes();
    for(int i=0; i<num; i++){
        ret.addByte(this->bytes[i]);
    }
    return ret;
}

Bytes Bytes::popFirstBytesFilledUp(const int num, const unsigned char fillup){
    Bytes firstbytes = this->getFirstBytesFilledUp(num, fillup);
    if(num < this->bytes.size()){
        auto first = this->bytes.begin() + num;
        auto last = this->bytes.end();
        this->bytes = std::vector<unsigned char>(first, last);
    }else{
        this->clear();
    }
    return firstbytes;
}

Bytes Bytes::getFirstBytesFilledUp(const int num, const unsigned char fillup) const{
    if(num < 0){
        throw std::range_error("The provided len is negative");
    }
    Bytes ret = Bytes();
    for(int i=0; i<num; i++){
        if(this->bytes.size() <= i){
            ret.addByte(fillup);
        }else{
            ret.addByte(this->bytes[i]);
        }
    }
    return ret;
}

bool Bytes::isEmpty() const noexcept{
    return this->getLen() == 0;
}

void Bytes::clear() noexcept{
    this->bytes.clear();
}


bool operator==(Bytes b1, Bytes b2){
    return (b1.bytes == b2.bytes);
}

Bytes operator+(Bytes b1, Bytes b2){
    if(b1.getLen() != b2.getLen()){
        throw std::length_error("bytes have different lengths");
    }
    Bytes ret = Bytes();
    for(int i=0; i < b1.getLen(); i++){
        ret.addByte(b1.getBytes()[i] + b2.getBytes()[i]);
    }
    return ret;
}

Bytes operator-(Bytes b1, Bytes b2){
    if(b1.getLen() != b2.getLen()){
        throw std::length_error("bytes have different lengths");
    }
    Bytes ret = Bytes();
    for(int i=0; i < b1.getLen(); i++){
        ret.addByte(b1.getBytes()[i] - b2.getBytes()[i]);
    }
    return ret;
}

std::string toHex(const unsigned char byte) noexcept{
    char hex[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    std::string ret{};
    ret += hex[(byte/16) % 16];
    ret += hex[byte % 16];
    return ret;
}

std::string toHex(Bytes b) noexcept{
    std::string ret{};
    for(unsigned char byte : b.getBytes()){
        ret += toHex(byte);
    }
    return ret;
}
