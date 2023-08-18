#include "password_data.h"

#include <algorithm>
#include <iomanip>
#include <map>

#include "logger.h"
#include "settings.h"
#include "utility.h"

ErrorStruct<bool> PasswordData::constructFileData(FileDataStruct& file_data) noexcept {
    // constructs the file data object with the file data struct
    PLOG_VERBOSE << "Constructing PasswordData object";
    if (file_data.getFileMode() != FILEMODE_PASSWORD) {
        return ErrorStruct<bool>{FAIL, ERR_FILEMODE_INVALID, std::to_string(+file_data.getFileMode()), "The file mode is not FileMode::PASSWORD_DATA"};
    }
    this->siteMap.clear();
    while (true) {
        // this loop iterates over the data sets
        if (file_data.dec_data.getLen() == 0) {
            // no more data sets
            break;
        }
        std::string site = "";
        for (int i = 0; i < 4; i++) {
            // this loop represents one data set
            std::optional<Bytes> size;
            try {
                size = file_data.dec_data.popFirstBytes(1);
            } catch (std::exception& e) {
                // should not happen
                PLOG_FATAL << "Something went wrong while getting the size byte: " << e.what();
                return ErrorStruct<bool>{FAIL, ERR_BUG, "Something went wrong while getting the size byte", e.what()};
            }
            if (!size.has_value()) {
                // some size byte is missing
                std::string type;
                switch (i) {
                    case 0:
                        type = "site";
                        break;
                    case 1:
                        type = "user";
                        break;
                    case 2:
                        type = "email";
                        break;
                    case 3:
                        type = "password";
                        break;
                }
                PLOG_ERROR << "The file data is not in the right format. Missing size byte for the " << type << " data";
                return ErrorStruct<bool>{FAIL, ERR_FILEDATA_INVALID, "The file data is not in the right format. Missing size byte for the " + type + " data"};
            }
            // the size byte is there
            unsigned char size_val = size.value().getBytes()[0];
            std::optional<Bytes> data;
            try {
                data = file_data.dec_data.popFirstBytes(size_val);
            } catch (std::exception& e) {
                // should not happen
                PLOG_FATAL << "Something went wrong while getting the data bytes: " << e.what();
                return ErrorStruct<bool>{FAIL, ERR_BUG, "Something went wrong while getting the data bytes", e.what()};
            }
            if (!data.has_value()) {
                // some data is missing
                std::string type;
                switch (i) {
                    case 0:
                        type = "site";
                        break;
                    case 1:
                        type = "user";
                        break;
                    case 2:
                        type = "email";
                        break;
                    case 3:
                        type = "password";
                        break;
                }
                PLOG_ERROR << "The file data is not in the right format. Missing data for the " << type << " data";
                return ErrorStruct<bool>{FAIL, ERR_FILEDATA_INVALID, "The file data is not in the right format. Missing data for the " + type + " data"};
            }
            // the data is there
            std::string data_str = charVecToString(data.value().getBytes());
            if (i == 0) {
                site = data_str;
                if (this->siteMap.count(site) == 0) 
                    this->siteMap[site] = std::vector<PasswordSiteSet>();
                else
                    this->siteMap[site].push_back(PasswordSiteSet{});
            } else {
                switch (i) {
                case 1:
                    this->siteMap[site].back().setUsername(data_str);
                    break;
                
                case 2:
                    this->siteMap[site].back().setEmail(data_str);
                    break;

                case 3:
                    this->siteMap[site].back().setPassword(data_str);
                    break;
                
                default:
                    return ErrorStruct<bool>{FAIL, ERR_BUG, "Something went wrong while getting the data bytes", "The index is not valid: " + std::to_string(i)};
                }
            }
        }
    }
    return ErrorStruct<bool>{SUCCESS, NO_ERR, "", "", true};
}

FileDataStruct PasswordData::getFileData() const {
    // returns the file data struct
    PLOG_VERBOSE << "Getting FileDataStruct object";
    FileDataStruct file_data{FILEMODE_PASSWORD, this->getBytes()};
    return file_data;
}

Bytes PasswordData::getBytes() const {
    // returns the data in Bytes
    Bytes ret{};
    for (std::pair<std::string, std::vector<PasswordSiteSet>> item : this->siteMap) {
        // iterate over all data sets
        Bytes siteB{};
        siteB.setBytes(std::vector<unsigned char>(item.first.begin(), item.first.end()));
        // check if the data has the right length (note that one site can have multiple data sets, but each data set has to have the same length (3))
        if (item.second.size() % 3 != 0) {
            PLOG_FATAL << "The password data is not in the right format in the site: " << item.first;
            throw std::runtime_error("The password data is not in the right format in the site: " + item.first);
        }
        int i = 0;
        while (true) {
            // iterate over all data sets of one site
            ret.addByte(siteB.getLen());
            ret.addBytes(siteB);

            Bytes userB{};
            userB.setBytes(std::vector<unsigned char>(item.second[i].getUsername().begin(), item.second[i].getUsername().begin()));
            ret.addByte(userB.getLen());
            ret.addBytes(userB);

            Bytes emailB{};
            emailB.setBytes(std::vector<unsigned char>(item.second[i].getEmail().begin(), item.second[i].getEmail().begin()));
            ret.addByte(emailB.getLen());
            ret.addBytes(emailB);

            Bytes passwordB{};
            passwordB.setBytes(std::vector<unsigned char>(item.second[i].getPassword().begin(), item.second[i].getPassword().begin()));
            ret.addByte(passwordB.getLen());
            ret.addBytes(passwordB);

            i++;

            if (i == item.second.size())
                // all data sets of one site are processed
                break;
        }
    }
    return ret;
}

std::vector<PasswordSet> PasswordData::getSets(const std::string substring, const bool sorted) const noexcept {
    // returns a list of password sets that contain the substring
    // turn substring to lowercase
    std::string substring_ignore_case = substring;
    std::transform(substring_ignore_case.begin(), substring_ignore_case.end(), substring_ignore_case.begin(), [](unsigned char c) { return std::tolower(c); });

    std::vector<PasswordSet> passwordSets;

    if (substring.empty()) {
        // Put all password sets into a vector (no substring given)
        for (std::pair<std::string, std::vector<PasswordSiteSet>> set : this->siteMap){
            for(PasswordSiteSet siteSet : set.second){
                passwordSets.push_back(PasswordSet{set.first, siteSet.getUsername(), siteSet.getEmail(), siteSet.getPassword()});
            }
        }
    } else {
        // Put all password sets that contain the substring into a vector
        for (std::pair<std::string, std::vector<PasswordSiteSet>> set : this->siteMap) {
            std::string first_ignore_case = set.first;
            std::transform(first_ignore_case.begin(), first_ignore_case.end(), first_ignore_case.begin(), [](unsigned char c) { return std::tolower(c); });
            if (first_ignore_case.find(substring_ignore_case) != std::string::npos){
                // The substring is in the site name
                for(PasswordSiteSet siteSet : set.second){
                    passwordSets.push_back(PasswordSet{set.first, siteSet.getUsername(), siteSet.getEmail(), siteSet.getPassword()});
                }
            }
        }
    }
    // Sort password sets in alphabetical order
    if(sorted)
        std::sort(passwordSets.begin(), passwordSets.end(), [](const auto& a, const auto& b) { return a.site < b.site; });
    return passwordSets;
}

void PasswordData::addPw(const PasswordSet pwset) {
    if(!pwset.isValid())
        throw std::invalid_argument("The given password set is not valid (at least one datapoint is too long)");
    if (this->siteMap.count(pwset.site) == 0)
        this->siteMap[pwset.site] = std::vector<PasswordSiteSet>();
    this->siteMap[pwset.site].push_back(PasswordSiteSet{pwset.username, pwset.email, pwset.password});
}

void PasswordData::remPw(const PasswordSet pwset) const noexcept {
    // WORK
}

void PasswordData::editPw(const PasswordSet pwset) const noexcept {
    // WORK
}
