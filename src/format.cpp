/*
contains the implementations of the format class
*/

#include "format.h"

#include "chainhash_modes.h"
#include "logger.h"

std::string Format::getFormatString(const CHModes& chainhash_mode) const {
    // gets the format string for a given chainhash mode
    switch (chainhash_mode) {
        case CHAINHASH_NORMAL:
            return "";
        case CHAINHASH_CONSTANT_SALT:
            return "*B S";
        case CHAINHASH_COUNT_SALT:
            return "8B SN";
        case CHAINHASH_CONSTANT_COUNT_SALT:
            return "8B SN *B S";
        case CHAINHASH_QUADRATIC:
            return "8B SN 8B A 8B B 8B C";
        default:  // invalid chainhash mode, should not throw
            PLOG_FATAL << "invalid chainhash mode, implementation missing (chainhash_mode: " << chainhash_mode << ")";
            return "error type";
    }
}

std::vector<NameLen> Format::calcNameLenList(const CHModes& chainhash_mode) const {
    // calculates the NameLen list for a given chainhash mode
    switch (chainhash_mode) {
        case CHAINHASH_NORMAL:
            return std::vector<NameLen>();
        case CHAINHASH_CONSTANT_SALT:
            return std::vector<NameLen>{NameLen{"S", 0}};
        case CHAINHASH_COUNT_SALT:
            return std::vector<NameLen>{NameLen{"SN", 8}};
        case CHAINHASH_CONSTANT_COUNT_SALT:
            return std::vector<NameLen>{NameLen{"SN", 8}, NameLen{"S", 0}};
        case CHAINHASH_QUADRATIC:
            return std::vector<NameLen>{NameLen{"SN", 8}, NameLen{"A", 8}, NameLen{"B", 8}, NameLen{"C", 8}};
        default:  // invalid chainhash mode, should not throw
            PLOG_FATAL << "invalid chainhash mode, implementation missing (chainhash_mode: " << chainhash_mode << ")";
            return std::vector<NameLen>();
    }
}

Format::Format(const CHModes chainhash_mode) : chainhash_mode(chainhash_mode), format(getFormatString(chainhash_mode)), name_lens(calcNameLenList(chainhash_mode)) {
    // basic constructor sets up the format string for the given chainhash mode
    if (!ChainHashModes::isModeValid(chainhash_mode)) {
        // given chainhash mode is not valid
        PLOG_FATAL << "given chainhash mode is not valid (chainhash_mode: " << chainhash_mode << ")";
        throw std::invalid_argument("given chainhash mode is not valid");
    }
}

CHModes Format::getChainMode() const noexcept { return this->chainhash_mode; }

std::vector<NameLen> Format::getNameLenList() const {
    // gets a List with NameLens structs
    return this->name_lens;
}

bool operator==(const Format& f1, const Format& f2) {
    // compare two formats by their format strings
    return f1.format == f2.format;
}
