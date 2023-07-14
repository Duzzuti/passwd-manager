#include "password_data.h"

#include <algorithm>
#include <map>

#include "iomanip"
#include "settings.h"
#include "utility.h"

bool PasswordData::getData(Bytes bytes) noexcept {
    // takes the bytes as an input and returns a bool that represents success of this function
    // the actual data gets stored in the siteMap
    while (true) {
        std::optional<Bytes> siteLenB = bytes.popFirstBytes(1);
        if (!siteLenB.has_value()) {
            break;
        }
        unsigned char siteLen = siteLenB.value().getBytes()[0];
        std::optional<Bytes> site = bytes.popFirstBytes(siteLen);
        std::optional<Bytes> userLenB = bytes.popFirstBytes(1);

        std::string site_str = charVecToString(site.value().getBytes());
        if (site_str[0] == '-') {
            this->error = "The site " + site_str + " starts with the illegal char '-'";
            return false;
        }
        if (!userLenB.has_value()) {
            this->siteMap.clear();
            this->error = "The dataset with the site name " + site_str + " contains no userLen byte";
            return false;
        }
        unsigned char userLen = userLenB.value().getBytes()[0];
        std::optional<Bytes> user = bytes.popFirstBytes(userLen);
        std::optional<Bytes> emailLenB = bytes.popFirstBytes(1);
        if (!emailLenB.has_value()) {
            this->siteMap.clear();
            this->error = "The dataset with the site name " + site_str + " contains no emailLen byte";
            return false;
        }
        unsigned char emailLen = emailLenB.value().getBytes()[0];
        std::optional<Bytes> email = bytes.popFirstBytes(emailLen);
        std::optional<Bytes> passwordLenB = bytes.popFirstBytes(1);
        if (!passwordLenB.has_value()) {
            this->siteMap.clear();
            this->error = "The dataset with the site name " + site_str + " contains no passwordLen byte";
            return false;
        }
        unsigned char passwordLen = passwordLenB.value().getBytes()[0];
        std::optional<Bytes> password = bytes.popFirstBytes(passwordLen);
        if (!password.has_value()) {
            this->siteMap.clear();
            this->error = "The dataset with the site name " + site_str + " contains not the full password";
            return false;
        }
        if (!(siteLen == site.value().getLen() && userLen == user.value().getLen() && emailLen == email.value().getLen() && passwordLen == password.value().getLen())) {
            this->siteMap.clear();
            this->error = "The dataset with the site name " + site_str + " has not matching datablock lengths";
            return false;
        }
        if (siteMap.count(site_str) > 0) {
            this->siteMap.clear();
            this->error = "The site name " + site_str + " has multiple occurrences";
            return false;
        }
        std::vector<std::string> dataV = {charVecToString(user.value().getBytes()), charVecToString(email.value().getBytes()), charVecToString(password.value().getBytes())};
        this->siteMap[site_str] = dataV;
    }
    return true;
}

void PasswordData::printHelp() const noexcept {
    std::cout << std::endl;
    std::cout << "[HELP]" << std::endl;
}

Bytes PasswordData::getBytes() const {
    Bytes ret{};
    for (std::pair<std::string, std::vector<std::string>> item : this->siteMap) {
        Bytes siteB{};
        siteB.setBytes(std::vector<unsigned char>(item.first.begin(), item.first.end()));
        ret.addByte(siteB.getLen());
        ret.addBytes(siteB);

        if (0 <= siteB.getLen() && siteB.getLen() <= 255) {
            std::cout << "Error: Length is not valid (Site: " << item.first << ", length: " << siteB.getLen() << ")" << std::endl;
            throw std::length_error("the length of the site is not valid");
        }

        Bytes userB{};
        userB.setBytes(std::vector<unsigned char>(item.second[0].begin(), item.second[0].end()));
        ret.addByte(userB.getLen());
        ret.addBytes(userB);

        if (0 <= userB.getLen() && userB.getLen() <= 255) {
            std::cout << "Error: Length is not valid (User: " << item.second[0] << ", length: " << userB.getLen() << ")" << std::endl;
            throw std::length_error("the length of the user is not valid");
        }

        Bytes emailB{};
        emailB.setBytes(std::vector<unsigned char>(item.second[1].begin(), item.second[1].end()));
        ret.addByte(emailB.getLen());
        ret.addBytes(emailB);

        if (0 <= emailB.getLen() && emailB.getLen() <= 255) {
            std::cout << "Error: Length is not valid (Email: " << item.second[1] << ", length: " << emailB.getLen() << ")" << std::endl;
            throw std::length_error("the length of the email is not valid");
        }

        Bytes passwordB{};
        passwordB.setBytes(std::vector<unsigned char>(item.second[2].begin(), item.second[2].end()));
        ret.addByte(passwordB.getLen());
        ret.addBytes(passwordB);

        if (0 <= passwordB.getLen() && passwordB.getLen() <= 255) {
            std::cout << "Error: Length is not valid (Password: " << item.second[2] << ", length: " << passwordB.getLen() << ")" << std::endl;
            throw std::length_error("the length of the password is not valid");
        }
    }
    return ret;
}

void PasswordData::showSets(const std::string substring) const noexcept {
    std::string substring_ignore_case = substring;
    std::transform(substring_ignore_case.begin(), substring_ignore_case.end(), substring_ignore_case.begin(), [](unsigned char c) { return std::tolower(c); });

    // Put all password sets that contain the substring into a vector
    std::vector<std::pair<std::string, std::vector<std::string>>> passwordSets;
    if (substring.empty()) {
        for (auto& set : this->siteMap) passwordSets.emplace_back(set);
    } else {
        for (auto& set : this->siteMap) {
            std::string first_ignore_case = set.first;
            std::transform(first_ignore_case.begin(), first_ignore_case.end(), first_ignore_case.begin(), [](unsigned char c) { return std::tolower(c); });
            if (first_ignore_case.find(substring_ignore_case) != std::string::npos) passwordSets.emplace_back(set);
        }
    }

    // Sort password sets in alphabetical order
    std::sort(passwordSets.begin(), passwordSets.end(), [](const auto& a, const auto& b) { return a.first < b.first; });

    // Output password sets in a table-like manner
    const int divider = 25;
    std::cout << std::left << std::setw(divider) << "Website" << std::left << std::setw(divider) << "Username" << std::left << std::setw(divider) << "Email" << std::left << std::setw(divider)
              << "Password" << '\n';
    for (const auto& set : passwordSets) {
        std::cout << std::left << std::setw(divider) << set.first << std::left << std::setw(divider) << set.second[0] << std::left << std::setw(divider) << set.second[1] << std::left
                  << std::setw(divider) << set.second[2] << '\n';
    }
}

void PasswordData::addPw(const std::string input) const noexcept {
    // WORK
}

void PasswordData::remPw(const std::string input) const noexcept {
    // WORK
}

void PasswordData::editPw(const std::string input) const noexcept {
    // WORK
}

Bytes PasswordData::run(Bytes bytes) {
    std::cout << std::endl << std::endl;
    std::cout << "Performing Password data operations..." << std::endl;
    // takes the plain data Bytes and does some user actions with it
    if (!this->getData(bytes)) {
        std::cout << "The decoded file data is not in the right format" << std::endl;
        std::cout << "[ERROR] " << this->getError() << std::endl;
        return bytes;
    }
    while (true) {
        std::string inp;
        std::cout << "Enter an action ('-h'-get help, '-q'-quit, '-a'-add password, '-r'-remove password, '-e'-edit password, a sitename (substring) to get data for that site): ";
        getline(std::cin, inp);
        if (inp.empty()) {
            std::cout << "You missed some keys, but you got this. Try again" << std::endl;
            continue;
        }
        if (inp[0] == '-') {
            if (inp.length() >= 2) {
                if (inp[1] == 'q') {
                    break;
                } else if (inp[1] == 'h') {
                    this->printHelp();
                } else if (inp[1] == 'a') {
                    this->addPw(inp);
                } else if (inp[1] == 'r') {
                    this->remPw(inp);
                } else if (inp[1] == 'e') {
                    this->editPw(inp);
                } else {
                    std::cout << "invalid option '" << inp[1] << "'. Enter -h to get help" << std::endl;
                }
            } else {
                std::cout << "please enter a option after '-'. Enter -h to get help" << std::endl;
            }
        } else {
            this->showSets(inp);
        }
    }
    return this->getBytes();
}

std::string PasswordData::getError() const noexcept { return this->error; }
