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
    const CHModes chainhash_mode;          // the chainhash mode that the user provides with the constructor
    const std::string format;              // the format string depending on the chainhash mode
    const std::vector<NameLen> name_lens;  // a list of all expected datablock parts as NameLen
   private:
    // calculates the format list for a given chainhash mode
    std::string getFormatString(const CHModes& chainhash_mode) const;  // gets the format string for a given chainhash mode
    // calculates the NameLen list for a given chainhash mode
    std::vector<NameLen> calcNameLenList(const CHModes& chainhash_mode) const;

   public:
    // constructor has to set the chainhash_mode and the corresponding string
    Format& operator=(const Format& format) = delete;  // copy assignment is deleted
    Format(const CHModes chainhash_mode);

    // getter for the chainhash mode
    CHModes getChainMode() const noexcept;
    // gets the list of NameLens, these structs contain the name and length of each datablock part
    std::vector<NameLen> getNameLenList() const;

    // it is possible to compare two formats (depending on their format strings)
    friend bool operator==(const Format& f1, const Format& f2);
};
