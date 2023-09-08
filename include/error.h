#pragma once

#include <iostream>
#include <optional>

#include "base.h"
#include "logger.h"

enum ErrorCode {
    NO_ERR,
    NO_ERROR_WAS_SET,
    ERR,
    ERR_BUG,
    ERR_TIMEOUT,
    ERR_ARGUMENT_INVALID,
    ERR_FILEMODE_INVALID,
    ERR_HASHMODE_INVALID,
    ERR_CHAINHASH_MISSING_VALUES,
    ERR_CHAINHASH_MODE_INVALID,
    ERR_CHAINHASHMODE_FORMAT_INVALID,
    ERR_CHAINHASH_DATAPART_INVALID,
    ERR_CHAINHASH_DATABLOCK_OUTOFRANGE,
    ERR_CHAINHASH_DATABLOCK_ALREADY_COMPLETED,
    ERR_CHAINHASH1_INVALID,
    ERR_CHAINHASH2_INVALID,
    ERR_ITERATIONS_INVALID,
    ERR_RUNTIME_INVALID,
    ERR_DATABLOCK_NOT_COMPLETED,
    ERR_DATABLOCK_TOO_LONG,
    ERR_PASSWD_CHAR_INVALID,
    ERR_PASSWORD_INVALID,
    ERR_LEN_INVALID,
    ERR_PASSWD_TOO_SHORT,
    ERR_EMPTY_FILEPATH,
    ERR_EXTENSION_INVALID,
    ERR_FILEPATH_INVALID,
    ERR_FILE_EXISTS,
    ERR_FILE_NOT_FOUND,
    ERR_FILE_NOT_DELETED,
    ERR_FILE_NOT_CREATED,
    ERR_FILE_NOT_OPEN,
    ERR_FILE_NOT_EMPTY,
    ERR_FILE_READ,
    ERR_NOT_ENOUGH_DATA,
    ERR_WRONG_WORKFLOW,
    ERR_API_NOT_INITIALIZED,
    ERR_API_STATE_INVALID,
    ERR_DATAHEADERSETTINGS_INCOMPLETE,
    ERR_FILEDATASTRUCT_INCOMPLETE,
    ERR_FILEDATA_INVALID,
    ERR_FILEDATASTRUCT_NULL
};

// used in a function that could fail, it returns a success type, a value and an error message
template <typename T>
struct ErrorStruct {
   private:
    bool moved = false;        // true if the value was moved
    bool input_moved = false;  // true if the input value was moved into the struct
   public:
    SuccessType success = FAIL;              // true if the operation was successful
    ErrorCode errorCode = NO_ERROR_WAS_SET;  // error code
    std::string errorInfo;                   // some specific data about the error can be stored here
    std::string what;                        // throw message

    static ErrorStruct<T> createMove(T&& value) {
        // returns a successful ErrorStruct
        ErrorStruct<T> err;
        err.success = SUCCESS;
        err.errorCode = NO_ERR;
        err.errorInfo = "";
        err.what = "";
        err.returnvalue = std::move(value);
        err.moved = false;
        err.input_moved = true;
        return err;
    };

    // constructors
    ErrorStruct() {
        // default constructor
        success = FAIL;
        errorCode = NO_ERROR_WAS_SET;
        errorInfo = "";
        what = "";
        moved = false;
        input_moved = false;
    };
    ErrorStruct(SuccessType success, ErrorCode errorCode, std::string errorInfo, std::string what, T returnvalue)
        : success(success), errorCode(errorCode), errorInfo(errorInfo), what(what), returnvalue(returnvalue){};
    ErrorStruct(SuccessType success, ErrorCode errorCode, std::string errorInfo) : success(success), errorCode(errorCode), errorInfo(errorInfo){};
    ErrorStruct(SuccessType success, ErrorCode errorCode, std::string errorInfo, std::string what) : success(success), errorCode(errorCode), errorInfo(errorInfo), what(what){};
    ErrorStruct(T returnvalue) : success(SuccessType::SUCCESS), errorCode(ErrorCode::NO_ERR), errorInfo(""), what(""), returnvalue(returnvalue){};

    // getters and setters
    T returnValue() {
        // returns by value
        // you cannot access the return value if the function failed
        if (moved) {
            PLOG_FATAL << "Cannot access the return value, because the value was moved.";
            throw std::logic_error("Cannot access the return value, because the value was moved.");
        }
        if (input_moved) {
            PLOG_FATAL << "Cannot access the return value, because the input value was moved into the struct. Try using returnRef() or returnMove() instead";
            throw std::logic_error("Cannot access the return value, because the input value was moved into the struct. Try using returnRef() or returnMove() instead");
        }
        if (!isSuccess()) {
            PLOG_FATAL << "Cannot access the return value, because the struct is not successful";
            throw std::logic_error("Cannot access the return value, because the struct is not successful");
        }
        if (!returnvalue.has_value()) {
            PLOG_ERROR << "Return value is not set";
            throw std::logic_error("Return value is not set");
        }
        return returnvalue.value();
    };
    T& returnRef() {
        // returns by reference
        // you cannot access the return value if the function failed
        if (moved) {
            PLOG_FATAL << "Cannot access the return value, because the value was moved.";
            throw std::logic_error("Cannot access the return value, because the value was moved.");
        }
        if (!isSuccess()) {
            PLOG_FATAL << "Cannot access the return value, because the struct is not successful";
            throw std::logic_error("Cannot access the return value, because the struct is not successful");
        }
        if (!returnvalue.has_value()) {
            PLOG_ERROR << "Return value is not set";
            throw std::logic_error("Return value is not set");
        }
        return returnvalue.value();
    };
    T&& returnMove() {
        // returns by move
        // you cannot access the return value if the function failed
        if (!isSuccess()) {
            PLOG_FATAL << "Cannot access the return value, because the struct is not successful";
            throw std::logic_error("Cannot access the return value, because the struct is not successful");
        }
        if (!returnvalue.has_value()) {
            PLOG_ERROR << "Return value is not set";
            throw std::logic_error("Return value is not set");
        }
        moved = true;
        return std::move(returnvalue.value());
    };
    bool isSuccess() { return success == SUCCESS; };
    void setReturnValue(T value) { returnvalue = value; };

   private:
    std::optional<T> returnvalue;  // return value
};

// returns an error message based on the error code and the error info
template <typename T>
std::string getErrorMessage(ErrorStruct<T>& err, bool verbose_err_msg = true) noexcept {
    // verbose_err_msg triggers the return of a more detailed error message
    //  returns an error message based on the error code and the error info
    if (err.isSuccess()) {
        PLOG_WARNING << "getErrorMessage was called on a succeeded ErrorStruct";
        return "getErrorMessage was called on a succeeded ErrorStruct";
    }
    std::string err_msg = "";
    if (verbose_err_msg) err_msg = "\nException message: " + err.what;

    switch (err.errorCode) {
        case NO_ERROR_WAS_SET:
            return "Some developer forgot to set the right Error type" + err_msg;

        case NO_ERR:
            return "No error occurred" + err_msg;

        case ERR_BUG:
            return "An error occurred due to a bug in the program. Some checks failed: " + err.errorInfo + err_msg;

        case ERR_TIMEOUT:
            return "A timeout occurred: " + err_msg;

        case ERR_ARGUMENT_INVALID:
            return "An argument is invalid: " + err.errorInfo + err_msg;

        case ERR_PASSWORD_INVALID:
            return "Password is invalid" + err_msg;

        case ERR_CHAINHASH_MISSING_VALUES:
            return "Chainhash has missing values" + err_msg;

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

        case ERR_CHAINHASH2_INVALID:
            return "Chainhash2 is invalid" + err_msg;

        case ERR_HASHMODE_INVALID:
            return "Hashmode is invalid: " + err.errorInfo + err_msg;

        case ERR_ITERATIONS_INVALID:
            return "Iterations number is invalid" + err_msg;

        case ERR_RUNTIME_INVALID:
            return "Runtime (in ms) is invalid" + err_msg;

        case ERR_LEN_INVALID:
            return "Invalid length from object: " + err.errorInfo + err_msg;

        case ERR_DATABLOCK_NOT_COMPLETED:
            return "Datablock is not completed" + err_msg;

        case ERR_DATABLOCK_TOO_LONG:
            return "Datablock is too long" + err_msg;

        case ERR_PASSWD_CHAR_INVALID:
            return "Password contains illegal character: '" + err.errorInfo + "'" + err_msg;

        case ERR_PASSWD_TOO_SHORT:
            return "Password is too short, it has to be at least " + err.errorInfo + " characters long" + err_msg;

        case ERR_EMPTY_FILEPATH:
            if (err.errorInfo.empty()) return "Filepath is empty" + err_msg;
            return err.errorInfo + " filepath is empty" + err_msg;

        case ERR_FILEPATH_INVALID:
            return "Filepath is invalid: " + err.errorInfo + err_msg;

        case ERR_EXTENSION_INVALID:
            if (err.errorInfo.empty()) return "File extension is invalid" + err_msg;
            return err.errorInfo + " file extension is invalid" + err_msg;

        case ERR_FILE_EXISTS:
            if (err.errorInfo.empty()) return "File already exists" + err_msg;
            return err.errorInfo + " file already exists" + err_msg;

        case ERR_FILE_NOT_OPEN:
            if (err.errorInfo.empty()) return "File could not be opened" + err_msg;
            return err.errorInfo + " file could not be opened" + err_msg;

        case ERR_FILE_NOT_FOUND:
            if (err.errorInfo.empty()) return "File could not be found" + err_msg;
            return err.errorInfo + " file could not be found" + err_msg;

        case ERR_FILE_NOT_CREATED:
            if (err.errorInfo.empty()) return "File could not be created" + err_msg;
            return err.errorInfo + " file could not be created" + err_msg;

        case ERR_FILE_NOT_DELETED:
            if (err.errorInfo.empty()) return "File could not be deleted" + err_msg;
            return err.errorInfo + " file could not be deleted" + err_msg;

        case ERR_FILE_NOT_EMPTY:
            if (err.errorInfo.empty()) return "File is not empty" + err_msg;
            return err.errorInfo + " file is not empty" + err_msg;

        case ERR_FILE_READ:
            return "File could not be read: " + err.errorInfo + err_msg;

        case ERR_NOT_ENOUGH_DATA:
            return "Not enough data to read information: " + err.errorInfo + err_msg;

        case ERR_FILEMODE_INVALID:
            return "File mode is invalid: " + err.errorInfo + err_msg;

        case ERR_WRONG_WORKFLOW:
            return "Wrong workflow: " + err.errorInfo + err_msg;

        case ERR_API_NOT_INITIALIZED:
            return "API is not initialized. Method failed: " + err.errorInfo + err_msg;

        case ERR_API_STATE_INVALID:
            return "API is in the wrong state. Method failed: " + err.errorInfo + err_msg;

        case ERR_DATAHEADERSETTINGS_INCOMPLETE:
            return "DataHeaderSettings are incomplete" + err_msg;

        case ERR_FILEDATASTRUCT_INCOMPLETE:
            return "FileDataStruct is incomplete" + err_msg;

        case ERR_FILEDATA_INVALID:
            return "FileData is invalid: " + err.errorInfo + err_msg;

        case ERR_FILEDATASTRUCT_NULL:
            return "FileDataStruct is null. It was moved previously" + err_msg;

        case ERR:
            if (err.errorInfo.empty()) return "An error occurred" + err_msg;
            return err.errorInfo + err_msg;

        default:
            PLOG_FATAL << "Unknown error code: " << +err.errorCode;
            return "Unknown error" + err_msg;
    }
}
