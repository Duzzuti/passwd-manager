/*
implementation of error.h
*/

#include "error.h"

std::string getErrorMessage(ErrorCode error_code, std::string error_info) noexcept {
    // returns an error message based on the error code and the error info
    switch (error_code) {
        case NO_ERR:
            return "No error occurred";

        case ERR_CHAINHASH_MODE_INVALID:
            return "Chainhash mode is invalid";

        case ERR_ITERATIONS_INVALID:
            return "Iterations number is invalid";

        case ERR_DATABLOCK_NOT_COMPLETED:
            return "Datablock is not completed";

        case ERR_DATABLOCK_TOO_LONG:
            return "Datablock is too long";

        case ERR_INVALID_PASSWD_CHAR:
            return "Password contains illegal character: '" + error_info + "'";

        case ERR_PASSWD_TOO_SHORT:
            return "Password is too short, it has to be at least " + error_info + " characters long";

        default:
            return "Unknown error";
    }
}
