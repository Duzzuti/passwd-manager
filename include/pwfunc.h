#pragma once
#ifndef PWFUNC_H
#define PWFUNC_H

#include "hash.h"

class PwFunc{
    /*
    this class provides methods that can be used to turn a password string unsigned longo a hash.
    Its using different chainhash methods.
    A chainhash is a hashfunction that is applied to the password and again to the result hash.
    This will loop for a given number of iterations. If you perform the same chainhash with the same password,
    you will get to the same hash.
    */
private:
    const Hash* hash;       //stores the hash function that should be used
    std::string error;      //stores the last occured error
public:
    //checks the password for illegal characters and length
    bool isPasswordValid(const std::string password) noexcept;

    std::string getError() const noexcept;  //gets the last occured error

    PwFunc() = default;
    PwFunc(const Hash* hash) noexcept;      //sets the hash function
    Bytes chainhash(const std::string password, const unsigned long iterations=1) const noexcept;       //performs a chainhash
    //adds a constant salt each iteration
    Bytes chainhashWithConstantSalt(const std::string password, const unsigned long iterations=1, const std::string salt="") const noexcept;
    //adds a salt (number that counts up each iteration)
    Bytes chainhashWithCountSalt(const std::string password, const unsigned long iterations=1, unsigned long salt_start=1) const noexcept;
    //adds a constant and count salt each iteration
    Bytes chainhashWithCountAndConstantSalt(const std::string password, const unsigned long iterations=1, unsigned long salt_start=1, const std::string salt="") const noexcept;
    //adds a quadratic count salt each iteration
    Bytes chainhashWithQuadraticCountSalt(const std::string password, const unsigned long iterations=1, unsigned long salt_start=1, const unsigned long a=1, const unsigned long b=1, const unsigned long c=1) const noexcept;
    
    //overload with Bytes data
    Bytes chainhash(const Bytes data, const unsigned long iterations=1) const noexcept;       //performs a chainhash
    //adds a constant salt each iteration
    Bytes chainhashWithConstantSalt(const Bytes data, const unsigned long iterations=1, const std::string salt="") const noexcept;
    //adds a salt (number that counts up each iteration)
    Bytes chainhashWithCountSalt(const Bytes data, const unsigned long iterations=1, unsigned long salt_start=1) const noexcept;
    //adds a constant and count salt each iteration
    Bytes chainhashWithCountAndConstantSalt(const Bytes data, const unsigned long iterations=1, unsigned long salt_start=1, const std::string salt="") const noexcept;
    //adds a quadratic count salt each iteration
    Bytes chainhashWithQuadraticCountSalt(const Bytes data, const unsigned long iterations=1, unsigned long salt_start=1, const unsigned long a=1, const unsigned long b=1, const unsigned long c=1) const noexcept;


};

#endif //PWFUNC_H