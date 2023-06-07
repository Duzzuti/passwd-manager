#pragma once

#include "base.h"
#include "bytes.h"
#include "dataheader.h"
#include "rng.h"
#include "settings.h"

// struct stores return data of the attacker run
// the attacker tries to crack the encryption and returns this struct
struct AttackerReturn {
    SuccessType success_type;  // was the attack successful
    u_int64_t tries;           // how many passwords were tried
    u_int64_t progress;        // how much progress was made (depends on the attacker, what that means)
    Bytes passwordhash;        // the got passwordhash if the attack was successful
};

class BaseAttacker {
    /*
    abstract class, all attackers have to inherit from this class and implement the attack method
    tests the encryption system against an attacker (to set recommended requirements for security)
    */
   protected:
    const std::string CHAR_SET = VALID_PASS_CHARSET;  // contains the valid char set (chars that can be used for the password)
   public:
    // template wrapper method that calls attack and checks the returns
    bool run(DataHeader dh, Bytes data, std::string decrypted_content, Bytes passwordhash) const;
    // WORK add time limit
    // virtual attack method, has to get implemented by derived classes;
    // testing the worst case scenario: the attacker got information about the dataheader parts the saved data encrypted and decrypted
    virtual AttackerReturn attack(DataHeaderParts dhp, Bytes data, std::string decrypted_content) const noexcept = 0;
};
