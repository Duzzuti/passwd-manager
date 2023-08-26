#include "block_opt_decrypt.h"

#include "logger.h"

void DecryptBlockOpt::addData(const BytesOpt& enc_data) {
    // add the data to the block
    if (this->getFreeSpace() < enc_data.getLen()) {	
        // the block data length will exceed the DecryptBlock length if the data is added
        PLOG_ERROR << "block data length will exceed the DecryptBlock length (block_len: " 
            << this->block_len << ", current_data_len: " << this->data.getLen() << ", add_data_len: " << enc_data.getLen() << ")";
        throw std::length_error("block data length will exceed the DecryptBlock length");
    }
    enc_data.addcopyToBytes(this->data);
    if (this->getFreeSpace() == 0) {
        // block is completed
        // calculate the block hash of the decrypted data
        if(this->dec_hash.getLen() != 0){
            // block hash was calculated previously. The only way that should happen is if the block is completed and we are adding empty data
            PLOG_WARNING << "block hash was calculated previously, added empty data (block_len: " << this->block_len << ", data_len: " << this->data.getLen() << ")";
        }else{
            this->data = this->data - this->salt;
            this->dec_hash = this->hash->hash(this->data);
        }
    }
}

BytesOpt DecryptBlockOpt::getResult() const noexcept {
    // returns the decrypted data
    if (this->getFreeSpace() != 0) {
        // block is not completed
        // calculate the decrypted data
        return this->data - this->salt.copySubBytes(0, this->data.getLen());
    }
    // the block has been decrypted
    return this->data;
}