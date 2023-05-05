#pragma once
#ifndef PWFUNC_H
#define PWFUNC_H

#include "hash.h"

class PwFunc{
    /*
    this class provides methods that can be used to turn a password string into a hash.
    Its using different chainhash methods.
    A chainhash is a hashfunction that is applied to the password and again to the result hash.
    This will loop for a given number of iterations. If you perform the same chainhash with the same password,
    you will get to the same hash.
    */
private:
    const Hash* hash;       //stores the hash function that should be used
public:
    static bool isPasswordValid(std::string password) noexcept;

    PwFunc() = default;
    PwFunc(const Hash* hash) noexcept;      //sets the hash function
    Bytes chainhash(const std::string password, int iterations=1) const noexcept;       //performs a chainhash
    Bytes chainhashWithConstantSalt(const std::string password, int iterations=1, const std::string salt="") const noexcept;    //adds a constant salt each iteration
    Bytes chainhashWithCountSalt(const std::string password, int iterations=1, int salt_start=1) const noexcept;    //adds a salt (number that counts up each iteration)
    Bytes chainhashWithCountAndConstantSalt(const std::string password, int iterations=1, int salt_start=1, const std::string salt="") const noexcept;  //adds a constant and count salt each iteration
};

#endif //PWFUNC_H