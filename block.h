#include "bytes.h"
#include "gtest/gtest.h"
#ifndef BLOCK_H
#define BLOCK_H

class Block{
private:
    int len;
    Bytes data;
    Bytes salt;
    Bytes passwordhash;
    Bytes encoded;

private:
    Bytes getSalt() const noexcept;
    Bytes getPasswordHash() const noexcept;
    Bytes getData() const noexcept;
public:
    //encrypt
    Block();
    Block(int len, Bytes data, Bytes salt, Bytes password);
    Block(const Block&) = delete;
    void setLen(int len);
    void setData(Bytes data);
    void setPasswordHash(Bytes passwordhash);
    void setSalt(Bytes salt);
    int getLen() const noexcept;
    Bytes getEncoded() const noexcept;
    bool isReadyForEncode() const noexcept;
    void calcEncoded();
    bool isEncoded() const noexcept;

    //decrypt
    Block(Bytes encoded);
    void setEncoded(Bytes encoded);
    bool isReadyForDecode() const noexcept;
    void calcData();
    bool isDecoded() const noexcept;

    void clear() noexcept;

};

#endif //(BLOCK_H)