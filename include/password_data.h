#pragma once
#ifndef PWDDATA_H
#define PWDDATA_H

#include <unordered_map>

#include "file_data.h"

class PasswordData : public FileData {
   private:
    std::unordered_map<std::string, std::vector<std::string>> siteMap;
    std::string error;
    bool getData(const Bytes bytes) noexcept;
    void printHelp() const noexcept;
    Bytes getBytes() const;
    void showSets(const std::string substring) const noexcept;
    void addPw(const std::string input) const noexcept;
    void remPw(const std::string input) const noexcept;
    void editPw(const std::string input) const noexcept;

   public:
    Bytes run(const Bytes bytes);
    std::string getError() const noexcept;
};

#endif  // PWDDATA_H