#pragma once


#include <iostream>

enum ErrorCode { NO_ERR, ERR_CHAINHASH_MODE_INVALID, ERR_ITERATIONS_INVALID, ERR_DATABLOCK_NOT_COMPLETED, ERR_DATABLOCK_TOO_LONG, ERR_INVALID_PASSWD_CHAR, ERR_PASSWD_TOO_SHORT };

// used in a function that could fail, it returns a success type, a value and an error message
template <typename T>
struct ErrorStruct {
    bool success;                  // true if the operation was successful
    ErrorCode errorCode = NO_ERR;  // error code
    std::string errorInfo;         // some specific data about the error can be stored here
    T returnValue;                 // return value
};

// returns an error message based on the error code and the error info
std::string getErrorMessage(ErrorCode error_code, std::string error_info) noexcept;

