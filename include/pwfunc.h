#pragma once

#include <memory>

#include "error.h"
#include "hash.h"

struct TimedResult {
    u_int64_t iterations;
    Bytes result;
};

class PwFunc {
    /*
    this class provides methods that can be used to turn a password string u_int64_to a hash.
    Its using different chainhash methods.
    A chainhash is a hash function that is applied to the password and again to the result hash.
    This will loop for a given number of iterations. If you perform the same chainhash with the same password,
    you will get to the same hash.

    You can set a timeout (in ms) for the chainhash functions.
    timeout = 0 means no timeout (which is also the default)
    if the timeout is reached before the chainhash is ready calculating it returns with a TIMEOUT SuccessType
    */
   private:
    std::shared_ptr<Hash> hash;  // stores the hash function that should be used
   public:
    // checks the password for illegal characters and length
    static ErrorStruct<bool> isPasswordValid(const std::string password) noexcept;

    PwFunc(std::shared_ptr<Hash> hash) noexcept;                                                                                           // sets the hash function
    ErrorStruct<Bytes> chainhash(const std::string password, const u_int64_t iterations = 1, const u_int64_t timeout = 0) const noexcept;  // performs a chainhash
    // adds a constant salt each iteration
    ErrorStruct<Bytes> chainhashWithConstantSalt(const std::string password, const u_int64_t iterations = 1, const std::string salt = "", const u_int64_t timeout = 0) const noexcept;
    // adds a salt (number that counts up each iteration)
    ErrorStruct<Bytes> chainhashWithCountSalt(const std::string password, const u_int64_t iterations = 1, u_int64_t salt_start = 1, const u_int64_t timeout = 0) const noexcept;
    // adds a constant and count salt each iteration
    ErrorStruct<Bytes> chainhashWithCountAndConstantSalt(const std::string password, const u_int64_t iterations = 1, u_int64_t salt_start = 1, const std::string salt = "",
                                                         const u_int64_t timeout = 0) const noexcept;
    // adds a quadratic count salt each iteration
    ErrorStruct<Bytes> chainhashWithQuadraticCountSalt(const std::string password, const u_int64_t iterations = 1, u_int64_t salt_start = 1, const u_int64_t a = 1, const u_int64_t b = 1,
                                                       const u_int64_t c = 1, const u_int64_t timeout = 0) const noexcept;

    // overload with Bytes data
    ErrorStruct<Bytes> chainhash(const Bytes data, const u_int64_t iterations = 1, const u_int64_t timeout = 0) const noexcept;  // performs a chainhash
    // adds a constant salt each iteration
    ErrorStruct<Bytes> chainhashWithConstantSalt(const Bytes data, const u_int64_t iterations = 1, const std::string salt = "", const u_int64_t timeout = 0) const noexcept;
    // adds a salt (number that counts up each iteration)
    ErrorStruct<Bytes> chainhashWithCountSalt(const Bytes data, const u_int64_t iterations = 1, u_int64_t salt_start = 1, const u_int64_t timeout = 0) const noexcept;
    // adds a constant and count salt each iteration
    ErrorStruct<Bytes> chainhashWithCountAndConstantSalt(const Bytes data, const u_int64_t iterations = 1, u_int64_t salt_start = 1, const std::string salt = "",
                                                         const u_int64_t timeout = 0) const noexcept;
    // adds a quadratic count salt each iteration
    ErrorStruct<Bytes> chainhashWithQuadraticCountSalt(const Bytes data, const u_int64_t iterations = 1, u_int64_t salt_start = 1, const u_int64_t a = 1, const u_int64_t b = 1, const u_int64_t c = 1,
                                                       const u_int64_t timeout = 0) const noexcept;

    // TIMED VERSIONS

    TimedResult chainhashTimed(const std::string password, const u_int64_t timeout) const noexcept;  // performs a chainhash
    // adds a constant salt each iteration
    TimedResult chainhashWithConstantSaltTimed(const std::string password, const u_int64_t timeout, const std::string salt = "") const noexcept;
    // adds a salt (number that counts up each iteration)
    TimedResult chainhashWithCountSaltTimed(const std::string password, const u_int64_t timeout, u_int64_t salt_start = 1) const noexcept;
    // adds a constant and count salt each iteration
    TimedResult chainhashWithCountAndConstantSaltTimed(const std::string password, const u_int64_t timeout, u_int64_t salt_start = 1, const std::string salt = "") const noexcept;
    // adds a quadratic count salt each iteration
    TimedResult chainhashWithQuadraticCountSaltTimed(const std::string password, const u_int64_t timeout, u_int64_t salt_start = 1, const u_int64_t a = 1, const u_int64_t b = 1,
                                                     const u_int64_t c = 1) const noexcept;

    // overload with Bytes data
    TimedResult chainhashTimed(const Bytes data, const u_int64_t timeout) const noexcept;  // performs a chainhash
    // adds a constant salt each iteration
    TimedResult chainhashWithConstantSaltTimed(const Bytes data, const u_int64_t timeout, const std::string salt = "") const noexcept;
    // adds a salt (number that counts up each iteration)
    TimedResult chainhashWithCountSaltTimed(const Bytes data, const u_int64_t timeout, u_int64_t salt_start = 1) const noexcept;
    // adds a constant and count salt each iteration
    TimedResult chainhashWithCountAndConstantSaltTimed(const Bytes data, const u_int64_t timeout, u_int64_t salt_start = 1, const std::string salt = "") const noexcept;
    // adds a quadratic count salt each iteration
    TimedResult chainhashWithQuadraticCountSaltTimed(const Bytes data, const u_int64_t timeout, u_int64_t salt_start = 1, const u_int64_t a = 1, const u_int64_t b = 1,
                                                     const u_int64_t c = 1) const noexcept;
};
