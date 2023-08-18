#pragma once

#include <unordered_map>

#include "file_data.h"

struct PasswordSet {
    std::string site;
    std::string username;
    std::string email;
    std::string password;

    bool isValid() const noexcept {
        if (site.size() > 255){
            PLOG_WARNING << "The site is too long, len: " << site.size();
            return false;
        }
        if (username.size() > 255){
            PLOG_WARNING << "The username is too long, len: " << username.size();
            return false;
        }
        if (email.size() > 255){
            PLOG_WARNING << "The email is too long, len: " << email.size();
            return false;
        }
        if (password.size() > 255){
            PLOG_WARNING << "The password is too long, len: " << password.size();
            return false;
        }
        return true;
    }
};

struct PasswordSiteSet {
   private:
    std::string username;
    std::string email;
    std::string password;
   public:
    PasswordSiteSet() = default;
    PasswordSiteSet(const std::string username, const std::string email, const std::string password) {
        this->setUsername(username);
        this->setEmail(email);
        this->setPassword(password);
    };
    
    void setUsername(const std::string username) {
        if (username.size() > 255){
            PLOG_FATAL << "The username is too long, len: " << username.size();
            throw std::invalid_argument("The username is too long");
        }
        this->username = username;
    };

    void setEmail(const std::string email) {
        if (email.size() > 255){
            PLOG_FATAL << "The email is too long, len: " << email.size();
            throw std::invalid_argument("The email is too long");
        }
        this->email = email;
    };

    void setPassword(const std::string password) {
        if (password.size() > 255){
            PLOG_FATAL << "The password is too long, len: " << password.size();
            throw std::invalid_argument("The password is too long");
        }
        this->password = password;
    };

    std::string getUsername() const noexcept {
        return this->username;
    };

    std::string getEmail() const noexcept {
        return this->email;
    };

    std::string getPassword() const noexcept {
        return this->password;
    };
};

class PasswordData : public FileData {
   private:
    // contains for each site sets of password data, one set contains a username, an email and a password. Multiple sets can be stored for one site
    std::unordered_map<std::string, std::vector<PasswordSiteSet>> siteMap;
    Bytes getBytes() const;

   public:
    // checks if the decrypted data is valid for it's use case. Checks if the file mode is correct
    // formats the byte data into the right datatypes (like maps and lists)
    ErrorStruct<bool> constructFileData(FileDataStruct& file_data) noexcept override final;

    // returns a file data struct that contains the current data of the file
    FileDataStruct getFileData() const override final;

    // returns the file mode that corresponds to the file data object
    FModes getFileMode() const noexcept override final { return FILEMODE_PASSWORD; };

    // returns the password data for all sites that contain the substring
    std::vector<PasswordSet> getSets(const std::string substring, const bool sorted) const noexcept;
    void addPw(const PasswordSet pwset);
    void remPw(const PasswordSet pwset) const noexcept;
    void editPw(const PasswordSet input) const noexcept;
};
