#pragma once

#include <memory>

#include "base.h"
#include "chainhash_data.h"
#include "error.h"
#include "hash.h"
#include "settings.h"

// forward declarations
struct ChainHash;
struct ChainHashTimed;
struct ChainHashResult;

class ChainHashModes {
    /*
    this class provides static methods all around chainhashes
    you can perform a chainhash or check if a chainhash mode is valid etc.
    A chainhash is a hash function that is applied multiple times
    Each chainhash mode has an own way of applying the hash function
    We can add some salt before we hash again and this salt is calculated differently in each mode
    */
   public:
    static std::string getInfo(const CHModes& chainhash_mode);        // gets a string that contains information about this chainhash mode
    static std::string getShortInfo(const CHModes& chainhash_mode);   // gets a short string that contains information about this chainhash mode
    static bool isModeValid(const CHModes& chainhash_mode) noexcept;  // checks if the given chain hash mode is valid
    // two methods for actually performing the chainhash, one for Bytes input and one for string input
    // expensive methods, you can set an timeout (in ms). 0 means no timeout.
    static ErrorStruct<Bytes> performChainHash(const ChainHash& chainh, std::shared_ptr<Hash> hash, const Bytes& data, const u_int64_t timeout = 0);
    static ErrorStruct<Bytes> performChainHash(const ChainHash& chainh, std::shared_ptr<Hash> hash, const std::string& data, const u_int64_t timeout = 0);
    // two other methods for actually performing the chainhash, one for Bytes input and one for string input
    // these methods use a runtime instead of iterations and are returning a ChainHash
    static ErrorStruct<ChainHashResult> performChainHash(const ChainHashTimed& chainh, std::shared_ptr<Hash> hash, const Bytes& data);
    static ErrorStruct<ChainHashResult> performChainHash(const ChainHashTimed& chainh, std::shared_ptr<Hash> hash, const std::string& data);
};

// ChainHash struct holds all components that are needed to describe a chainhash
struct ChainHash {
   private:
    std::optional<CHModes> mode{};       // the mode
    ChainHashData* datablock = nullptr;  // the data corresponding to the chainhash
    u_int64_t iters = 0;                 // the iters
    bool val = false;                    // is this chainhash valid?

    // checks if the given chainhash is valid (with the iterations and datablock which contains data that is used by the chainhash)
    ErrorStruct<bool> isValid() noexcept {
        // checks if the chainhash is valid
        this->val = false;
        if (!this->mode.has_value() || this->datablock == nullptr) {
            PLOG_WARNING << "The chainhash is invalid (not all values are set)";
            PLOG_DEBUG << *this;
            return ErrorStruct<bool>{FAIL, ERR_CHAINHASH_MISSING_VALUES, ""};
        }
        ErrorStruct<bool> es;
        es.success = FAIL;
        if (!(this->iters > 0 && this->iters <= MAX_ITERATIONS)) {
            PLOG_WARNING << "chainhash iterations are not valid (" << this->iters << ")";
            PLOG_DEBUG << *this;
            es.errorCode = ERR_ITERATIONS_INVALID;
            return es;  // iteration number is not valid
        }
        if (!this->datablock->isCompletedFormat(Format(this->mode.value()))) {
            PLOG_WARNING << "chainhash datablock is not completed or has the wrong format";
            PLOG_DEBUG << *this;
            // checks if the datablock is already completed
            es.errorCode = ERR_DATABLOCK_NOT_COMPLETED;
            return es;
        }
        if (this->datablock->getLen() > 255) {
            PLOG_WARNING << "chainhash datablock is too long (" << this->datablock->getLen() << ")";
            PLOG_DEBUG << *this;
            es.errorCode = ERR_DATABLOCK_TOO_LONG;
            es.success = FAIL;
            return es;  // datablock is too long
        }
        es.success = SUCCESS;
        this->val = true;
        return es;
    }

    // setters
    void _setIters(const u_int64_t iters) {
        // sets the iters
        this->iters = iters;
    };
    void _setMode(const CHModes chainhash_mode) {
        // sets the chainhash mode
        if (!ChainHashModes::isModeValid(chainhash_mode)) {
            PLOG_FATAL << "chainhash mode is not valid (" << +chainhash_mode << ")";
            throw std::invalid_argument("chainhash mode does not exist");
        }
        this->mode = chainhash_mode;
    };
    void _setChainHashData(ChainHashData chainhash_data) {
        // sets the chainhash data block
        this->datablock = &chainhash_data;
    };

   public:
    ChainHash& operator=(const ChainHash& ch) {
        this->_setMode(ch.getMode());
        this->_setIters(iters);
        this->_setChainHashData(ch.getChainHashData());
        this->isValid();
        return *this;
    };
    ChainHash() = default;
    ChainHash(CHModes chainhash_mode, u_int64_t iters, const ChainHashData& chainhash_data) {
        // constructor for a chainhash
        // this chainhash is validated
        this->_setMode(chainhash_mode);
        this->_setIters(iters);
        this->_setChainHashData(chainhash_data);
        this->isValid();
    }

    // setters
    void setIters(const u_int64_t iters) {
        // sets the iters
        this->_setIters(iters);
        this->isValid();
    };
    void setMode(const CHModes chainhash_mode) {
        // sets the chainhash mode
        this->_setMode(chainhash_mode);
        this->isValid();
    };
    void setChainHashData(const ChainHashData& chainhash_data) {
        // sets the chainhash data block
        this->_setChainHashData(chainhash_data);
        this->isValid();
    };

    // getters only work if the chainhash is valid
    CHModes getMode() const {
        // returns the chainhash mode
        if (!this->val) {
            PLOG_FATAL << "The chainhash is invalid (getMode)";
            throw std::runtime_error("The chainhash is invalid (getMode)");
        }
        return this->mode.value();
    };
    ChainHashData getChainHashData() const {
        // returns the chainhash data block
        if (!this->val) {
            PLOG_FATAL << "The chainhash is invalid (getChainHashData)";
            throw std::runtime_error("The chainhash is invalid (getChainHashData)");
        }
        return *this->datablock;
    };
    u_int64_t getIters() const {
        // returns the iters
        if (!this->val) {
            PLOG_FATAL << "The chainhash is invalid (getIters)";
            throw std::runtime_error("The chainhash is invalid (getIters)");
        }
        return this->iters;
    };

    bool valid() const noexcept {
        // returns if the chainhash is valid
        return this->val;
    }

    friend std::ostream& operator<<(std::ostream& os, const ChainHash& ch) {
        // prints the chainhash parts
        return os << "[ChainHash] "
                  << "ch_mode: " << (ch.mode.has_value() ? std::to_string(+ch.mode.value()) : "not set") << ", "
                  << "iters: " << std::to_string(ch.iters) << ", "
                  << "datablock: " << (ch.datablock != nullptr ? (ch.datablock->isComplete() ? ch.datablock->getDataBlock().toHex() : "not completed") : "not set") << ", "
                  << "valid: " << ch.val;
    }
};

// ChainHashTimed struct holds all components that are needed to describe a chainhash
// this chainhash does not contain iterations but a runtime to get the iterations
struct ChainHashTimed {
   private:
    std::optional<CHModes> mode;         // the mode
    ChainHashData* datablock = nullptr;  // the data corresponding to the chainhash
    u_int64_t run_time;                  // the time that this chainhash should run (in ms)
    bool val = false;                    // is this chainhash valid?

    // checks if the given chainhash is valid (with the runtime and datablock which contains data that is used by the chainhash)
    ErrorStruct<bool> isValid() noexcept {
        // checks if the chainhash is valid
        this->val = false;
        if (!this->mode.has_value() || this->datablock == nullptr) {
            PLOG_WARNING << "The chainhash is invalid (not all values are set)";
            PLOG_DEBUG << *this;
            return ErrorStruct<bool>{FAIL, ERR_CHAINHASH_MISSING_VALUES, ""};
        }
        ErrorStruct<bool> es;
        es.success = FAIL;
        if (!(this->run_time > 0 && this->run_time <= MAX_RUNTIME)) {
            PLOG_WARNING << "chainhash run time is not valid (" << this->run_time << ")";
            PLOG_DEBUG << *this;
            es.errorCode = ERR_RUNTIME_INVALID;
            return es;  // run time is not valid
        }
        if (!this->datablock->isCompletedFormat(Format(this->mode.value()))) {
            PLOG_WARNING << "chainhash datablock is not completed or has the wrong format";
            PLOG_DEBUG << *this;
            // checks if the datablock is already completed
            es.errorCode = ERR_DATABLOCK_NOT_COMPLETED;
            return es;
        }
        if (this->datablock->getLen() > 255) {
            PLOG_WARNING << "chainhash datablock is too long (" << this->datablock->getLen() << ")";
            PLOG_DEBUG << *this;
            es.errorCode = ERR_DATABLOCK_TOO_LONG;
            es.success = FAIL;
            return es;  // datablock is too long
        }
        es.success = SUCCESS;
        this->val = true;
        return es;
    }

    // setters
    void _setRunTime(const u_int64_t run_time) {
        // sets the run_time
        this->run_time = run_time;
    };
    void _setMode(const CHModes chainhash_mode) {
        // sets the chainhash mode
        if (!ChainHashModes::isModeValid(chainhash_mode)) {
            PLOG_FATAL << "chainhash mode is not valid (" << +chainhash_mode << ")";
            throw std::invalid_argument("chainhash mode does not exist");
        }
        this->mode = chainhash_mode;
    };
    void _setChainHashData(ChainHashData chainhash_data) {
        // sets the chainhash data block
        this->datablock = &chainhash_data;
    };

   public:
    ChainHashTimed& operator=(const ChainHashTimed& ch) {
        this->_setMode(ch.getMode());
        this->_setRunTime(ch.getRunTime());
        this->_setChainHashData(ch.getChainHashData());
        this->isValid();
        return *this;
    };
    ChainHashTimed() = default;
    ChainHashTimed(CHModes chainhash_mode, u_int64_t run_time_ms, const ChainHashData& chainhash_data) {
        // constructor for a chainhash
        // this chainhash is validated
        this->_setMode(chainhash_mode);
        this->_setRunTime(run_time_ms);
        this->_setChainHashData(chainhash_data);
        this->isValid();
    }

    // setters
    void setRunTime(const u_int64_t run_time_ms) {
        // sets the run time in ms
        this->_setRunTime(run_time_ms);
        this->isValid();
    };
    void setMode(const CHModes chainhash_mode) {
        // sets the chainhash mode
        this->_setMode(chainhash_mode);
        this->isValid();
    };
    void setChainHashData(const ChainHashData& chainhash_data) {
        // sets the chainhash data block
        this->_setChainHashData(chainhash_data);
        this->isValid();
    };

    // getters only work if the chainhash is valid
    CHModes getMode() const {
        // returns the chainhash mode
        if (!this->val) {
            PLOG_FATAL << "The chainhash is invalid (getMode)";
            throw std::runtime_error("The chainhash is invalid (getMode)");
        }
        return this->mode.value();
    };
    ChainHashData getChainHashData() const {
        // returns the chainhash data block
        if (!this->val) {
            PLOG_FATAL << "The chainhash is invalid (getChainHashData)";
            throw std::runtime_error("The chainhash is invalid (getChainHashData)");
        }
        return *this->datablock;
    };
    u_int64_t getRunTime() const {
        // returns the run time in ms
        if (!this->val) {
            PLOG_FATAL << "The chainhash is invalid (getRunTime)";
            throw std::runtime_error("The chainhash is invalid (getRunTime)");
        }
        return this->run_time;
    };

    bool valid() const noexcept {
        // returns if the chainhash is valid
        return this->val;
    }
    friend std::ostream& operator<<(std::ostream& os, const ChainHashTimed& ch) {
        // prints the chainhashtimed parts
        return os << "[ChainHashTimed] "
                  << "ch_mode: " << (ch.mode.has_value() ? std::to_string(+ch.mode.value()) : "not set") << ", "
                  << "run_time: " << std::to_string(ch.run_time) << ", "
                  << "datablock: " << (ch.datablock != nullptr ? (ch.datablock->isComplete() ? ch.datablock->getDataBlock().toHex() : "not completed") : "not set") << ", "
                  << "valid: " << ch.val;
    }
};

// ChainHashResult struct holds the actual chainhash that was used and the result of the chainhash
// this is returned if a timed chainhash was performed
struct ChainHashResult {
    ChainHash chainhash;  // the actual chainhash
    Bytes result;         // the result of the chainhash
};