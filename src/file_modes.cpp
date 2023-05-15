#include <sstream>
#include "file_modes.h"
#include "settings.h"

std::string FileModes::getInfo(unsigned char const file_mode) noexcept{
    std::stringstream msg{}; 
    msg << "The file mode " << +file_mode << " stores ";
    switch (file_mode){
        case 1: //password data
            msg << "password data";
            break;
    }
    return msg.str();
}

bool FileModes::isModeValid(unsigned char const file_mode) noexcept{
    //checks whether the given file mode is in the accepted range
    return (1 <= file_mode && file_mode <= MAX_FILEMODE_NUMBER);
}
