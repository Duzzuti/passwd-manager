#include <iostream>

#include "logger.h"
#include "api.h"


int main(int argc, char* argv[]) {
    // for (int i = 0; i < argc; i++){
    //     std::cout << "a"<<argv[i]<<"b" << std::endl;
    //     if (argv[i] == "-f"){
    //         if (argc == i){
    //             std::cout << "break";
    //             return 1;
    //         }
    //         std::cout << "no break";
    //         i++;
    //         char* path = argv[i];
    //         FileHandler FH;
    //         std::cout << FH.setFilePath(path);
    //         continue;
    //     }
    // }

    // init logger
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    plog::init(plog::verbose, &consoleAppender);

    PLOG_INFO << "Starting application";

    API api{FModes::FILEMODE_PASSWORD};

    PLOG_INFO << "Application finished";
}
