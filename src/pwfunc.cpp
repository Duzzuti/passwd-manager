#include "pwfunc.h"

#include "logger.h"
#include "settings.h"
#include "timer.h"
#include "utility.h"

ErrorStruct<bool> PwFunc::isPasswordValid(const std::string password) noexcept {
    // checks the password for illegal format, sets an error if it is not valid
    ErrorStruct<bool> ret;
    for (int i = 0; i < password.length(); i++) {
        // loops through the password characters
        bool found = false;
        for (int j = 0; j < VALID_PASS_CHARSET.length(); j++) {
            if (password[i] == VALID_PASS_CHARSET[j]) {  // checks if the character is in the valid chars
                found = true;
                break;  // character found
            }
        }
        if (!found) {  // invalid character, is not in charset
            PLOG_WARNING << "invalid character found in password (char: " << password[i] << ", ascii: " << +password[i] << ")";
            ret.errorCode = ERR_PASSWD_CHAR_INVALID;
            ret.errorInfo = password[i];
            ret.success = FAIL;  // passwd contains illegal char
            return ret;
        }
    }
    if (password.length() < MIN_PASS_LEN) {  // password length does not match the min length
        PLOG_WARNING << "password is too short (length: " << password.length() << ", min length: " << MIN_PASS_LEN << ")";
        ret.errorCode = ERR_PASSWD_TOO_SHORT;
        ret.errorInfo = std::to_string(MIN_PASS_LEN);
        ret.success = FAIL;  // passwd too short
        return ret;
    }
    ret.setReturnValue(true);
    ret.success = SUCCESS;  // passwd is valid
    return ret;
}

PwFunc::PwFunc(std::shared_ptr<Hash> hash) noexcept { this->hash = std::move(hash); }

ErrorStruct<Bytes> PwFunc::chainhash(const std::string password, const u_int64_t iterations, const u_int64_t timeout) const noexcept {
    Timer timer;
    timer.start();
    Bytes ret = this->hash->hash(password);  // hashes the password
    for (u_int64_t i = 1; i < iterations; i++) {
        // for iterations -1 the hash is hashed again
        ret = this->hash->hash(ret);
        if (timeout != 0 && i % TIMEOUT_ITERATIONS == 0) {
            if (timeout <= timer.peekTime()) {
                PLOG_WARNING << "timeout reached (timeout: " << timeout << ", iterations: " << i << ")";
                return ErrorStruct<Bytes>{TIMEOUT, ERR_TIMEOUT, "", "", Bytes()};
            }
        }
    }
    return ErrorStruct<Bytes>{SUCCESS, NO_ERR, "", "", ret};
}

ErrorStruct<Bytes> PwFunc::chainhashWithConstantSalt(const std::string password, const u_int64_t iterations, const std::string salt, const u_int64_t timeout) const noexcept {
    Timer timer;
    timer.start();
    Bytes ret = this->hash->hash(password + salt);  // hashes the password with the salt added
    for (u_int64_t i = 1; i < iterations; i++) {
        // for iterations -1 the salt is added to the current hash and the result is hashed again
        ret.addBytes(stringToBytes(salt));
        ret = this->hash->hash(ret);
        if (timeout != 0 && i % TIMEOUT_ITERATIONS == 0) {
            if (timeout <= timer.peekTime()) {
                PLOG_WARNING << "timeout reached (timeout: " << timeout << ", iterations: " << i << ")";
                return ErrorStruct<Bytes>{TIMEOUT, ERR_TIMEOUT, "", "", Bytes()};
            }
        }
    }
    return ErrorStruct<Bytes>{SUCCESS, NO_ERR, "", "", ret};
}

ErrorStruct<Bytes> PwFunc::chainhashWithCountSalt(const std::string password, const u_int64_t iterations, u_int64_t salt_start, const u_int64_t timeout) const noexcept {
    Timer timer;
    timer.start();
    Bytes ret = this->hash->hash(password + std::to_string(salt_start));  // hashes the password with the start salt added
    for (u_int64_t i = 1; i < iterations; i++) {
        // for iterations - 1 the salt will count up and gets added to the current hash and is hashed again
        salt_start++;
        ret.addBytes(stringToBytes(std::to_string(salt_start)));
        ret = this->hash->hash(ret);
        if (timeout != 0 && i % TIMEOUT_ITERATIONS == 0) {
            if (timeout <= timer.peekTime()) {
                PLOG_WARNING << "timeout reached (timeout: " << timeout << ", iterations: " << i << ")";
                return ErrorStruct<Bytes>{TIMEOUT, ERR_TIMEOUT, "", "", Bytes()};
            }
        }
    }
    return ErrorStruct<Bytes>{SUCCESS, NO_ERR, "", "", ret};
}

ErrorStruct<Bytes> PwFunc::chainhashWithCountAndConstantSalt(const std::string password, const u_int64_t iterations, u_int64_t salt_start, const std::string salt,
                                                             const u_int64_t timeout) const noexcept {
    Timer timer;
    timer.start();
    Bytes ret = this->hash->hash(password + salt + std::to_string(salt_start));  // the password is hashed with the salt and the count salt
    for (u_int64_t i = 1; i < iterations; i++) {
        // for iterations -1 the count salt will increment. The constant salt gets added to the current hash as well as the count salt
        // the result is hashed again
        salt_start++;
        ret.addBytes(stringToBytes(salt + std::to_string(salt_start)));
        ret = this->hash->hash(ret);
        if (timeout != 0 && i % TIMEOUT_ITERATIONS == 0) {
            if (timeout <= timer.peekTime()) {
                PLOG_WARNING << "timeout reached (timeout: " << timeout << ", iterations: " << i << ")";
                return ErrorStruct<Bytes>{TIMEOUT, ERR_TIMEOUT, "", "", Bytes()};
            }
        }
    }
    return ErrorStruct<Bytes>{SUCCESS, NO_ERR, "", "", ret};
}

ErrorStruct<Bytes> PwFunc::chainhashWithQuadraticCountSalt(const std::string password, const u_int64_t iterations, u_int64_t salt_start, const u_int64_t a, const u_int64_t b, const u_int64_t c,
                                                           const u_int64_t timeout) const noexcept {
    Timer timer;
    timer.start();
    Bytes ret = this->hash->hash(password + std::to_string(a * salt_start * salt_start + b * salt_start + c));  // hashes the password with the a*start_salt^2 + b*start_salt + c added
    for (u_int64_t i = 1; i < iterations; i++) {
        // for iterations - 1 the salt will count up and its quadratic value is added to the current hash and is hashed again
        salt_start++;
        ret.addBytes(stringToBytes(std::to_string(a * salt_start * salt_start + b * salt_start + c)));
        ret = this->hash->hash(ret);
        if (timeout != 0 && i % TIMEOUT_ITERATIONS == 0) {
            if (timeout <= timer.peekTime()) {
                PLOG_WARNING << "timeout reached (timeout: " << timeout << ", iterations: " << i << ")";
                return ErrorStruct<Bytes>{TIMEOUT, ERR_TIMEOUT, "", "", Bytes()};
            }
        }
    }
    return ErrorStruct<Bytes>{SUCCESS, NO_ERR, "", "", ret};
}

ErrorStruct<Bytes> PwFunc::chainhash(const Bytes data, const u_int64_t iterations, const u_int64_t timeout) const noexcept {
    Timer timer;
    timer.start();
    Bytes ret = data;
    for (u_int64_t i = 0; i < iterations; i++) {
        // for iterations the hash is hashed again
        ret = this->hash->hash(ret);
        if (timeout != 0 && i % TIMEOUT_ITERATIONS == 0) {
            if (timeout <= timer.peekTime()) {
                PLOG_WARNING << "timeout reached (timeout: " << timeout << ", iterations: " << i << ")";
                return ErrorStruct<Bytes>{TIMEOUT, ERR_TIMEOUT, "", "", Bytes()};
            }
        }
    }
    return ErrorStruct<Bytes>{SUCCESS, NO_ERR, "", "", ret};
}

ErrorStruct<Bytes> PwFunc::chainhashWithConstantSalt(const Bytes data, const u_int64_t iterations, const std::string salt, const u_int64_t timeout) const noexcept {
    Timer timer;
    timer.start();
    Bytes ret = data;
    for (u_int64_t i = 0; i < iterations; i++) {
        // for iterations the salt is added to the current hash and the result is hashed again
        ret.addBytes(stringToBytes(salt));
        ret = this->hash->hash(ret);
        if (timeout != 0 && i % TIMEOUT_ITERATIONS == 0) {
            if (timeout <= timer.peekTime()) {
                PLOG_WARNING << "timeout reached (timeout: " << timeout << ", iterations: " << i << ")";
                return ErrorStruct<Bytes>{TIMEOUT, ERR_TIMEOUT, "", "", Bytes()};
            }
        }
    }
    return ErrorStruct<Bytes>{SUCCESS, NO_ERR, "", "", ret};
}

ErrorStruct<Bytes> PwFunc::chainhashWithCountSalt(const Bytes data, const u_int64_t iterations, u_int64_t salt_start, const u_int64_t timeout) const noexcept {
    Timer timer;
    timer.start();
    Bytes ret = data;
    for (u_int64_t i = 0; i < iterations; i++) {
        // for iterations the salt will count up and is added to the current hash and is hashed again
        ret.addBytes(stringToBytes(std::to_string(salt_start)));
        ret = this->hash->hash(ret);
        salt_start++;
        if (timeout != 0 && i % TIMEOUT_ITERATIONS == 0) {
            if (timeout <= timer.peekTime()) {
                PLOG_WARNING << "timeout reached (timeout: " << timeout << ", iterations: " << i << ")";
                return ErrorStruct<Bytes>{TIMEOUT, ERR_TIMEOUT, "", "", Bytes()};
            }
        }
    }
    return ErrorStruct<Bytes>{SUCCESS, NO_ERR, "", "", ret};
}

ErrorStruct<Bytes> PwFunc::chainhashWithCountAndConstantSalt(const Bytes data, const u_int64_t iterations, u_int64_t salt_start, const std::string salt, const u_int64_t timeout) const noexcept {
    Timer timer;
    timer.start();
    Bytes ret = data;
    for (u_int64_t i = 0; i < iterations; i++) {
        // for iterations the count salt will increment. The constant salt gets added to the current hash as well as the count salt
        // the result is hashed again
        ret.addBytes(stringToBytes(salt + std::to_string(salt_start)));
        ret = this->hash->hash(ret);
        salt_start++;
        if (timeout != 0 && i % TIMEOUT_ITERATIONS == 0) {
            if (timeout <= timer.peekTime()) {
                PLOG_WARNING << "timeout reached (timeout: " << timeout << ", iterations: " << i << ")";
                return ErrorStruct<Bytes>{TIMEOUT, ERR_TIMEOUT, "", "", Bytes()};
            }
        }
    }
    return ErrorStruct<Bytes>{SUCCESS, NO_ERR, "", "", ret};
}

ErrorStruct<Bytes> PwFunc::chainhashWithQuadraticCountSalt(const Bytes data, const u_int64_t iterations, u_int64_t salt_start, const u_int64_t a, const u_int64_t b, const u_int64_t c,
                                                           const u_int64_t timeout) const noexcept {
    Timer timer;
    timer.start();
    Bytes ret = data;
    for (u_int64_t i = 0; i < iterations; i++) {
        // for iterations the salt will count up and its quadratic value is added to the current hash and is hashed again
        ret.addBytes(stringToBytes(std::to_string(a * salt_start * salt_start + b * salt_start + c)));
        ret = this->hash->hash(ret);
        salt_start++;
        if (timeout != 0 && i % TIMEOUT_ITERATIONS == 0) {
            if (timeout <= timer.peekTime()) {
                PLOG_WARNING << "timeout reached (timeout: " << timeout << ", iterations: " << i << ")";
                return ErrorStruct<Bytes>{TIMEOUT, ERR_TIMEOUT, "", "", Bytes()};
            }
        }
    }
    return ErrorStruct<Bytes>{SUCCESS, NO_ERR, "", "", ret};
}