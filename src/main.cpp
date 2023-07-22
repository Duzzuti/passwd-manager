#include <iostream>

#include "api.h"
#include "logger.h"

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
    PLOG_DEBUG << "API created";
    ErrorStruct<std::filesystem::path> dir = api.getEncDirPath();
    PLOG_DEBUG << "Getting enc dir path: " << dir.returnValue();
    ErrorStruct<std::vector<std::string>> paths = api.getAllEncFileNames(dir.returnValue());
    PLOG_DEBUG << "Getting all enc file names: " << paths.returnValue().size();
    for (auto& path : paths.returnValue()) {
        PLOG_DEBUG << path;
    }
    //WORK

    PLOG_INFO << "Application finished";
}
