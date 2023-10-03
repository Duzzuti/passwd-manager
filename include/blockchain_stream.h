#pragma once
#include <fstream>
#include <memory>
#include <optional>

#include "block.h"
#include "logger.h"

class BlockChainStream {
    /*
    The blockchain stream class represents a optimized blockchain class
    it will not store the blocks in a vector but stream the data from a file to a file

    A BlockChainStream is initialized with a hash function, a password hash and an encrypted salt
    The hash function should be got from the DataHeader´s hash mode
    The password hash is the hash of the correct password
    The encrypted salt is got from the DataHeader

    The BlockChainStream class is abstract, because it uses different blocks for encryption and decryption

    You can stream new data that should be encrypted/decrypted with enc_stream
    that will add the data to the last block of the chain and if the block is completed it will write the result to the output file
    for each block there is a salt generated that is used to encrypt/decrypt the data
    the salt is generated with the password hash and the encrypted salt as well as the last blocks hash
    this is done with the SaltIterator class
    It is:
        - deterministic
        - you can only get the next salt if you have the last block hash
        - if the first salt is known to the attacker and even if he knows the data of the first block
        he cannot get the next salt or passwordhash (except he knows the inverse hash function :))
    */
   protected:
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
            this->salt = enc_salt;
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
            // note that the salt is not equal to the hash because the salt is generated with the new hash
            this->salt = this->hashObj->hash(this->hash + this->salt + last_block_hash);
            // return a hash of the hash and the salt to make sure you cannot calculate any of the other values
            return this->hashObj->hash(this->hash + this->salt);
        }
    };

    u_int64_t height;                                          // the height of the chain
    const size_t hash_size;                                    // the byte size of the hash function
    std::optional<std::unique_ptr<Block>> current_block = {};  // the current block
    Bytes last_block_hash;                                     // the hash of the last block
    SaltIterator salt_iter;                                    // the salt iterator that is used to generate the salts

   protected:
    // sets a new block and updates the last block hash from the current block
    virtual bool setBlock() noexcept = 0;
    // returns the free space in the block
    size_t getFreeSpaceInLastBlock() const noexcept;

   public:
    // creates a new empty blockchain with the hash function, the password hash and the encrypted salt
    BlockChainStream(std::shared_ptr<Hash> hash, const Bytes& passwordhash, const Bytes& enc_salt);
    BlockChainStream(const BlockChainStream&) = delete;
    BlockChainStream& operator=(const BlockChainStream&) = delete;
    BlockChainStream() = delete;

    // adds new data to the blockchain (streams the data encrypted/decrypted from the input file to the output file)
    void enc_stream(std::ifstream&& in, std::ofstream&& out) noexcept;

    // returns the number of blocks in the chain
    size_t getHeight() const noexcept { return this->height; };

    // returns the size of the data in the chain (in Bytes)
    size_t getDataSize() const noexcept { return this->height * this->hash_size - this->getFreeSpaceInLastBlock(); };
};