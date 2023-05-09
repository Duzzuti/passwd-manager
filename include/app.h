#pragma once
#ifndef APP_H
#define APP_H

#include <iostream>
#include "filehandler.h"

class App{
private:
    std::string filePath;
    FileHandler FH;

private:
    void printStart();
    bool isValidMode(std::string mode, bool accept_blank=false) const noexcept;
    bool isValidNumber(std::string number, bool accept_blank=false) const noexcept;
    std::string askForPasswd() const noexcept;
    unsigned char askForMode() const noexcept;
    long askForPasswdIters() const noexcept;
public:
    App();
    bool run();
};

#endif //APP_H