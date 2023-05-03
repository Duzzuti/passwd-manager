#pragma once
#ifndef APP_H
#define APP_H

#include <iostream>

class App{
private:
    std::string filePath;
public:
    App();
    bool run();
    void printStart() const noexcept;
};

#endif //APP_H