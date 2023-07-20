/*
this file contains the implementations of FileModes class
*/
#include "file_modes.h"

#include <sstream>

#include "logger.h"

#include "password_data.h"
#include "settings.h"

std::string FileModes::getInfo(const FModes file_mode) {
    // gets some information about the provided file mode
    std::stringstream msg{};
    msg << "The file mode " << +file_mode << " stores ";  // start string
    // add mode specific strings
    switch (file_mode) {
        case FILEMODE_PASSWORD:  // password data
            msg << "password data";
            break;
        default:  // not an valid mode
            std::cout << msg.str();
            PLOG_ERROR << "Invalid file mode passed into get info file mode: " << +file_mode;
            throw std::logic_error("Invalid file mode passed into get info");
    }
    return msg.str();
}

bool FileModes::isModeValid(const FModes file_mode) noexcept {
    // checks whether the given file mode is in the accepted range
    return (1 <= file_mode && file_mode <= MAX_FILEMODE_NUMBER);
}
