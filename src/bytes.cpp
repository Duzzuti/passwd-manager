#include <cmath>
#include "bytes.h"
#include "rng.h"

Bytes::Bytes(){
    this->clear();
}

Bytes::Bytes(const int len){
    if(len < 0){
        //invalid length given
        throw std::range_error("The provided len is negative");
    }
    //sets random bytes (with a cryptographically secure algorithm from openssl)
    this->setBytes(RNG().get_random_bytes(len));
}

void Bytes::print() const noexcept{
    std::cout << toHex(*this);  //prints itself as a hex string
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
        //adds each byte of the Bytes object
        this->addByte(byte);
    }
}

std::optional<Bytes> Bytes::popFirstBytes(const int num){
    std::optional<Bytes> firstbytes = this->getFirstBytes(num); //gets the first num bytes
    if(firstbytes.has_value()){
        //if it has returned a valid value (there were enough bytes left)
        auto first = this->bytes.begin() + num; //our new vector begins on the numth element
        auto last = this->bytes.end();      //to the end
        this->bytes = std::vector<unsigned char>(first, last);      //create a new vector with that bounds
    }
    return firstbytes;  //returns back the got values (or not got)
}

std::optional<Bytes> Bytes::getFirstBytes(const int num) const{
    if(num < 0){
        //how the programm should return the first negative elments?
        throw std::range_error("The provided len is negative");
    }
    if(num > this->bytes.size()){
        //if there are not enough bytes, its returning an empty optional
        return {};    
    }
    Bytes ret = Bytes();
    for(int i=0; i<num; i++){
        //add the first num bytes and returning the new byte object
        ret.addByte(this->bytes[i]);
    }
    return ret;
}

Bytes Bytes::popFirstBytesFilledUp(const int num, const unsigned char fillup){
    Bytes firstbytes = this->getFirstBytesFilledUp(num, fillup);
    if(num < this->bytes.size()){
        //if there were enough bytes we will create a new vector by setting the new bounds
        auto first = this->bytes.begin() + num;
        auto last = this->bytes.end();
        this->bytes = std::vector<unsigned char>(first, last);
    }else{
        //if there were not enough bytes the vector gets cleared up
        this->clear();
    }
    return firstbytes;
}

Bytes Bytes::getFirstBytesFilledUp(const int num, const unsigned char fillup) const{
    if(num < 0){
        //how the programm should return the first negative elments?
        throw std::range_error("The provided len is negative");
    }
    Bytes ret = Bytes();
    for(int i=0; i<num; i++){
        if(this->bytes.size() <= i){
            //if there are not enough elements, we add the fillup byte
            ret.addByte(fillup);
        }else{
            //otherwise we add the byte
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
    //compare the two byte vectors
    return (b1.bytes == b2.bytes);
}

Bytes operator+(Bytes b1, Bytes b2){
    if(b1.getLen() != b2.getLen()){
        //the two Bytes need to have the same length to perform an elementwise addition
        throw std::length_error("bytes have different lengths");
    }
    Bytes ret = Bytes();
    for(int i=0; i < b1.getLen(); i++){
        //appends the sum of the two bytes as a new byte (implicit mod 256) to the return byte
        ret.addByte(b1.getBytes()[i] + b2.getBytes()[i]);
    }
    return ret;
}

Bytes operator-(Bytes b1, Bytes b2){
    if(b1.getLen() != b2.getLen()){
        //the two Bytes need to have the same length to perform an elementwise subtraction
        throw std::length_error("bytes have different lengths");
    }
    Bytes ret = Bytes();
    for(int i=0; i < b1.getLen(); i++){
        //appends the difference of the two bytes as a new byte (implicit mod 256) to the return byte
        ret.addByte(b1.getBytes()[i] - b2.getBytes()[i]);
    }
    return ret;
}

std::string toHex(const unsigned char byte) noexcept{
    char hex[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    std::string ret{};
    ret += hex[(byte/16) % 16]; //the first hex (higher char)
    ret += hex[byte % 16];      //the second hex (lower char)
    return ret;
}

std::string toHex(Bytes b) noexcept{
    std::string ret{};
    for(unsigned char byte : b.getBytes()){
        //performs for each byte a transformation to hex string
        ret += toHex(byte);
    }
    return ret;
}

unsigned long toLong(const unsigned char byte) noexcept{
    return (long)byte;
}

unsigned long toLong(Bytes b) noexcept{
    unsigned long ret = 0;
    std::vector<unsigned char> v = b.getBytes();
    for(int i=0; i < v.size(); i++){
        //performs for each byte a transformation to long
        unsigned long byte_value = toLong(v[i]);
        unsigned long byte_position = std::pow(256, v.size()-i-1);
        ret += byte_value*byte_position;
    }
    return ret;
}
