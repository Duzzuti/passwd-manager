#pragma once
#ifndef BYTES_H
#define BYTES_H

#include <vector>
#include <optional>

class Bytes{
private:
    std::vector<unsigned char> bytes;
public:
    Bytes();
    Bytes(const int len);
    //Bytes(const Bytes&) = delete;
    void print() const noexcept;
    void setBytes(std::vector<unsigned char> bytes) noexcept;
    std::vector<unsigned char> getBytes() const noexcept;
    int getLen() const noexcept;
    void addByte(const unsigned char byte) noexcept;
    std::optional<Bytes> popFirstBytes(const int num);
    std::optional<Bytes> getFirstBytes(const int num) const;
};

std::string toHex(const unsigned char byte) noexcept;

#endif //BYTES_H