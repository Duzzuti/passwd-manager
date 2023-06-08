#pragma once

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
    // checks if the decrypted data is valid for it's use case. Checks if the file mode is correct
    // formats the byte data into the right datatypes (like maps and lists)
    ErrorStruct<bool> constructFileData(FileDataStruct file_data) noexcept override final;

    // returns a file data struct that contains the current data of the file
    FileDataStruct getFileData() const noexcept override final;

    // returns the file mode that corresponds to the file data object
    FModes getFileMode() const noexcept override final { return FILEMODE_PASSWORD; };

    Bytes run(const Bytes bytes);
    std::string getError() const noexcept;
};
