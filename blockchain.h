#include "block.h"
#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

class BlockChain{
private:
    Bytes encoded;
    Bytes data;
    std::vector<Block> chain;
public:
    BlockChain();
    BlockChain(const BlockChain&) = delete;
    //encrypt
    bool addBlock() noexcept;
    void completeBlockChain() noexcept;
    bool isBlockChainComplete() const noexcept;
    void addData(Bytes data) noexcept;
    void setData(Bytes data) noexcept;
    Bytes getEncoded() const;

    //decrypt
    void setEncoded(Bytes encoded) noexcept;
    

};

#endif //BLOCKCHAIN_H