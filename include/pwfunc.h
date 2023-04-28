#pragma once
#ifndef PWFUNC_H
#define PWFUNC_H

#include "hash.h"

class PwFunc{
private:
    const Hash* hash;
public:
    PwFunc(const Hash* hash) noexcept;
    Bytes chainhash(const std::string password, int iterations=1) const noexcept;
    Bytes chainhashWithConstantSalt(const std::string password, int iterations=1, const std::string salt="") const noexcept;
    Bytes chainhashWithCountSalt(const std::string password, int iterations=1, int salt_start=1) const noexcept;
    Bytes chainhashWithCountAndConstantSalt(const std::string password, int iterations=1, int salt_start=1, const std::string salt="") const noexcept;
};

#endif //PWFUNC_H