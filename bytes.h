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
    Bytes popFirstBytesFilledUp(const int num, const unsigned char fillup=0);
    Bytes getFirstBytesFilledUp(const int num, const unsigned char fillup=0) const;
    bool isEmpty() const noexcept;
    void clear() noexcept;
    friend bool operator==(Bytes b1, Bytes b2);
};

std::string toHex(const unsigned char byte) noexcept;

#endif //BYTES_H