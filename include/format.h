#pragma once


#include <iostream>
#include <vector>

#include "base.h"

class Format {
    /*
    the format class is providing functionality with chainhash data block formats
    every datablock can have different data depending on the format
    this class is working on the format string and is calculating and validating the format
    chainhash_modes.md
    */
   private:
    std::string format;      // the format string depending on the chainhash mode
    CHModes chainhash_mode;  // the chainhash mode that the user provides with the constructor
   public:
    // constructor has to set the chainhash_mode and the corresponding string
    Format(const CHModes chainhash_mode);

    // getter for the chainhash mode
    CHModes getChainMode() const noexcept;
    // calculates a list of NameLens, these structs contain the name and length of each datablock part
    std::vector<NameLen> getNameLenList() const;

    // it is possible to compare two formats (depending on their format strings)
    friend bool operator==(const Format& f1, const Format& f2);
};

