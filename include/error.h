#pragma once

#include <iostream>

#include "base.h"

enum ErrorCode {
    NO_ERR,
    ERR,
    ERR_FILEMODE_INVALID,
    ERR_HASHMODE_INVALID,
    ERR_CHAINHASH_MODE_INVALID,
    ERR_CHAINHASHMODE_FORMAT_INVALID,
    ERR_CHAINHASH_DATAPART_INVALID,
    ERR_CHAINHASH_DATABLOCK_OUTOFRANGE,
    ERR_CHAINHASH_DATABLOCK_ALREADY_COMPLETED,
    ERR_CHAINHASH1_INVALID,
    ERR_ITERATIONS_INVALID,
    ERR_DATABLOCK_NOT_COMPLETED,
    ERR_DATABLOCK_TOO_LONG,
    ERR_INVALID_PASSWD_CHAR,
    ERR_PASSWD_TOO_SHORT,
    ERR_EMPTY_FILEPATH,
    ERR_INVALID_FILEPATH,
    ERR_NOT_ENOUGH_DATA
};

// used in a function that could fail, it returns a success type, a value and an error message
template <typename T>
struct ErrorStruct {
    SuccessType success;           // true if the operation was successful
    ErrorCode errorCode = NO_ERR;  // error code
    std::string errorInfo;         // some specific data about the error can be stored here
    std::string what;              // throw message
    T returnValue;                 // return value
};

// returns an error message based on the error code and the error info
template <typename T>
std::string getErrorMessage(ErrorStruct<T> err) noexcept {
    // returns an error message based on the error code and the error info
    std::string err_msg = "\nException message: " + err.what;
    switch (err.errorCode) {
        case NO_ERR:
            return "No error occurred" + err_msg;

        case ERR_CHAINHASH_MODE_INVALID:
            return "Chainhash mode is invalid" + err_msg;

        case ERR_CHAINHASHMODE_FORMAT_INVALID:
            return "Chainhash mode format is invalid for chainhash mode: " + err.errorInfo + err_msg;

        case ERR_CHAINHASH_DATAPART_INVALID:
            return "Chainhash data part is invalid: " + err.errorInfo + err_msg;

        case ERR_CHAINHASH_DATABLOCK_OUTOFRANGE:
            return "Chainhash datablock is out of range due to data part: " + err.errorInfo + err_msg;

        case ERR_CHAINHASH_DATABLOCK_ALREADY_COMPLETED:
            return "Chainhash datablock is already completed. Following data part wants to add more data: " + err.errorInfo + err_msg;

        case ERR_CHAINHASH1_INVALID:
            return "Chainhash1 is invalid" + err_msg;

        case ERR_HASHMODE_INVALID:
            return "Hashmode is invalid: " + err.errorInfo + err_msg;

        case ERR_ITERATIONS_INVALID:
            return "Iterations number is invalid" + err_msg;

        case ERR_DATABLOCK_NOT_COMPLETED:
            return "Datablock is not completed" + err_msg;

        case ERR_DATABLOCK_TOO_LONG:
            return "Datablock is too long" + err_msg;

        case ERR_INVALID_PASSWD_CHAR:
            return "Password contains illegal character: '" + err.errorInfo + "'" + err_msg;

        case ERR_PASSWD_TOO_SHORT:
            return "Password is too short, it has to be at least " + err.errorInfo + " characters long" + err_msg;

        case ERR_EMPTY_FILEPATH:
            if (err.errorInfo.empty()) return "Filepath is empty" + err_msg;
            return err.errorInfo + " filepath is empty" + err_msg;

        case ERR_INVALID_FILEPATH:
            return "Filepath is invalid: " + err.errorInfo + err_msg;

        case ERR_NOT_ENOUGH_DATA:
            return "Not enough data to read information: " + err.errorInfo + err_msg;

        case ERR_FILEMODE_INVALID:
            return "File mode is invalid: " + err.errorInfo + err_msg;

        case ERR:
            if (err.errorInfo.empty()) return "An error occurred" + err_msg;
            return err.errorInfo + err_msg;

        default:
            return "Unknown error" + err_msg;
    }
}
