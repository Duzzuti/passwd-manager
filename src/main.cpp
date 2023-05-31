#include <iostream>

#include "app.h"

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
    App app;
    return app.run();
}
