#pragma once
#include "hash.h"
#include "logger.h"

class SaltIterator {
    /*
    the SaltIterator class is used to generate the salts for the blocks
    it is initialized with the password hash and the encrypted salt
    for every block it generates a new salt with the hash of the last block
    */
   private:
    bool ready;     // is the iterator ready to generate salts
    bool first;     // is this the first salt/block
    Bytes hash{0};  // the current hash (first is the passwordhash)
    Bytes salt{0};  // the current salt (first is the encrypted salt)
   public:
    std::shared_ptr<Hash> hashObj;  // the hash object that provides the hash function

   public:
    SaltIterator() {
        // not ready yet, has to be initialized first (with init)
        this->ready = false;
        this->first = true;
    }
    void init(const Bytes& pwhash, const Bytes& enc_salt, std::shared_ptr<Hash> hashObj) {
        // initializes the iterator with the password hash and the encrypted salt
        if (hashObj == nullptr) {
            PLOG_FATAL << "given hash object is nullptr";
            throw std::invalid_argument("hash cannot be nullptr");
        }
        if (pwhash.getLen() != hashObj->getHashSize()) {
            PLOG_FATAL << "passwordhash has to be the same size as the hash from the hash function (passwordhash_len: " << pwhash.getLen() << ", hash_size: " << hashObj->getHashSize() << ")";
            throw std::invalid_argument("passwordhash has to be the same size as the hash");
        }
        if (enc_salt.getLen() != hashObj->getHashSize()) {
            PLOG_FATAL << "encrypted salt has to be the same size as the hash from the hash function (enc_salt_len: " << enc_salt.getLen() << ", hash_size: " << hashObj->getHashSize() << ")";
            throw std::invalid_argument("enc_salt has to be the same size as the hash");
        }
        this->ready = true;
        this->hash = pwhash;
        this->salt = hashObj->hash(pwhash + enc_salt);
        this->hashObj = std::move(hashObj);
    }
    Bytes next(Bytes last_block_hash = Bytes(255)) {
        // generates the next salt with the last block hash
        if (this->first) {
            // if this is the first block, the last_block_hash is set to 0
            first = false;
            // generate a Bytes object with the size of the hash with all bytes set to 0
            unsigned char buffer[this->hashObj->getHashSize()];
            // will set the whole buffer to 0
            std::memset(buffer, 0, this->hashObj->getHashSize());
            last_block_hash.setBytes(buffer, this->hashObj->getHashSize());
        }
        if (last_block_hash.getLen() != this->hashObj->getHashSize()) {
            PLOG_FATAL << "last_block_hash has to be the same size as the hash from the hash function (last_block_hash_len: " << last_block_hash.getLen()
                       << ", hash_size: " << this->hashObj->getHashSize() << ")";
            throw std::invalid_argument("last_block_hash has to be the same size as the hash");
        }
        if (!this->ready) {
            PLOG_FATAL << "SaltIterator is not ready, call init first";
            throw std::runtime_error("SaltIterator is not ready, call init first");
        }
        // generate the next hash and salt by hashing the last hash and salt with the last block hash
        this->hash = this->hashObj->hash(this->hash + this->salt + last_block_hash);
        // return a hash of the hash and the salt to make sure you cannot calculate any of the other values
        return this->hashObj->hash(this->hash + this->salt);
    }
};