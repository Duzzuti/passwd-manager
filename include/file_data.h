#pragma once

#include "bytes.h"

class FileData {
    /*
    Interface class that holds methods that every FileData class has to implement
    Strategy pattern: we can handle all FileData classes the same
    A FileData class is a class that implements this interface. It is used for the semantik of the data
    If we decrypted the file's data, the user can do different actions dependend on the type of data that is stored in the file
    */
   private:
    // checks if the decrypted data is valid for it's use case
    // formats the byte data into the right datatypes (like maps and lists), returns a success bool
    virtual bool getData(Bytes bytes) noexcept = 0;

   public:
    // runs the data actions
    //(like an event loop where the user can enter the action he wants to do and the data gets shown or modified)
    // at the end, we will return the modified Bytes back to the user
    virtual Bytes run(Bytes) = 0;
    // returns the last occured error: all methods returning a success bool should overwrite the error if success is false
    virtual std::string getError() const noexcept = 0;  // gets the last error from other methods
};
