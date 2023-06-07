/*
this file contains some structs and enums that requires no import (excluding c++ built in libraries)
you can include this file everywhere without creating circular includes
*/
#pragma once

#include <iostream>

// enum which holds the chainhash modes
enum CHModes {
    CHAINHASH_NORMAL = 1,           // normal chainhash without salt
    CHAINHASH_CONSTANT_SALT,        // chainhash with constant salt
    CHAINHASH_COUNT_SALT,           // chainhash with count salt
    CHAINHASH_CONSTANT_COUNT_SALT,  // chainhash with constant and count salt
    CHAINHASH_QUADRATIC             // chainhash with quadratic salt
};

// enum which holds the hash modes
enum HModes {
    HASHMODE_SHA256 = 1,  // sha256 hashmode
    HASHMODE_SHA384,      // sha384 hashmode
    HASHMODE_SHA512,      // sha512 hashmode
};

// enum which holds the file data modes
enum FModes {
    FILEMODE_PASSWORD = 1,  // password filemode
};

// enum which describes a success type, an success type can be a success, fail or timeout
enum SuccessType { SUCCESS, FAIL, TIMEOUT };

// struct that is used as an data package between format and other classes
// it contains the name of the chainhash block part and its length in bytes (or zero if it is a *B part)
// chainhash_modes.md
struct NameLen {
   public:
    std::string name;   // name of the data (e.g A, SN)
    unsigned char len;  // length of the data part (e.g. 8 or 0)
    NameLen(std::string name, unsigned char len) {
        // basic constructor
        this->name = name;
        this->len = len;
    }
};
