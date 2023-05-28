/*
implementation of the abstract class base attacker
*/
#include "base_attacker.h"

bool BaseAttacker::run(DataHeader dh, Bytes data, std::string decrypted_content, Bytes passwordhash) const{
    //template method run that calls the attack method 
    //WORK start timer here
    //attacker gets header data, and encrypted/decrypted body
    AttackerReturn attack_ret = this->attack(dh.getDataHeaderParts(), data, decrypted_content);
    //end timer here
    bool success = false;
    switch(attack_ret.success_type){
        //did the attacker succeed?
        case SUCCESS:
            if(attack_ret.passwordhash == passwordhash){
                //actual success
                success = true;
                std::cout << "Oh noo, passwordhash found (Goodbye data)";
                break;
            }
            std::cout << "Passwordhash found, but it was invalid";
            break;
        case FAIL:
            //attacker failed
            std::cout << "Passwordhash not found, attacker gave up";
            break;
        case TIMEOUT:
            //attacker failed because of timeout
            std::cout << "Passwordhash not found due to timeout";
            break;
        default:
            //should never happen
            throw std::logic_error("Invalid success type");
    }
    std::cout << std::endl;
    std::cout << "Attacker tried " << attack_ret.tries << " passwords" << std::endl;
    std::cout << "Attacker has made progress up to " << attack_ret.progress << " (depends on the attacker what this means)" << std::endl;
    //WORK print the needed time
    if(success){
        std::cout << "Attacker got your passwordhash " << toHex(attack_ret.passwordhash) << std::endl;
        return true;
    }
    return false;
   
}