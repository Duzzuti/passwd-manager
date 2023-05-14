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
    std::string askForPasswd() const noexcept;
    unsigned char askForHashMode() const noexcept;
    unsigned char askForChainHashMode() const noexcept;
    unsigned long askForIters(std::string msg) const noexcept;
    Bytes askForHeader() const;
public:
    App();
    bool run();
    static bool isValidHashMode(std::string mode, bool accept_blank=false) noexcept;
    static bool isValidChainHashMode(std::string mode, bool accept_blank=false) noexcept;
    static bool isValidNumber(std::string number, bool accept_blank=false, unsigned long lower_bound=0, unsigned long upper_bound=-1) noexcept;
};

#endif //APP_H