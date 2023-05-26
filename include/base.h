/*
this file contains some structs and enums that requires no import (excluding c++ built in libraries)
you can include this file everywhere without creating circular includes
*/
#pragma once
#ifndef BASE_H
#define BASE_H

#include <iostream>

//enum which holds the chainhash modes as unisgned chars
enum CHModes : unsigned char{
    CHAINHASH_NORMAL = 1,       //normal chainhash without salt
    CHAINHASH_CONSTANT_SALT,    //chainhash with constant salt
    CHAINHASH_COUNT_SALT,       //chainhash with count salt
    CHAINHASH_CONSTANT_COUNT_SALT,  //chainhash with constant and count salt
    CHAINHASH_QUADRATIC         //chainhash with quadratic salt
};

//struct that is used as an data package between format and other classes
//it contains the name of the chainhash block part and its length in bytes (or zero if it is a *B part)
//chainhash_modes.md
struct NameLen{
public:
    std::string name;   //name of the data (e.g A, SN)
    unsigned char len;  //length of the data part (e.g. 8 or 0)
    NameLen(std::string name, unsigned char len){
        //basic constructor
        this->name = name;
        this->len = len;
    }
};

#endif //BASE_H