#include "gtest/gtest.h"
#include "dataheader.h"
#include "rng.h"
#include "test_settings.cpp"

TEST(DataHeaderClass, Constructor){
    //testing the dataheader constructor
    //creating a list of hash modes, where the first (0) and the last (MAX_HASHMODE +1) are invalid
    std::vector<unsigned char> hash_modes;
    for(int i=0; i <= MAX_HASHMODE_NUMBER+1; i++){
        hash_modes.push_back(i);
    }

    //testing the hash modes on the constructor
    for(int i=0; i < hash_modes.size(); i++){
        //throw testing
        if(i == 0 || i == hash_modes.size() - 1){
            EXPECT_THROW(DataHeader dh = DataHeader(hash_modes[i]), std::invalid_argument);
        }else{
            EXPECT_NO_THROW(DataHeader dh = DataHeader(hash_modes[i]));
        }
    }
}

TEST(DataHeaderClass, setFileDataMode){
    //testing the set file data mode from the dataheader
    //creating a list of file modes, where the first (0) and the last (MAX_FILEMODE +1) are invalid
    std::vector<unsigned char> file_modes;
    for(int i=0; i <= MAX_FILEMODE_NUMBER+1; i++){
        file_modes.push_back(i);
    }
    //setting up the dataheader object
    DataHeader dh = DataHeader(STANDARD_HASHMODE);

    //testing the file modes on the setter
    for(int i=0; i < file_modes.size(); i++){
        //throw testing
        if(i == 0 || i == file_modes.size() - 1){
            EXPECT_THROW(dh.setFileDataMode(file_modes[i]), std::invalid_argument);
        }else{
            EXPECT_NO_THROW(dh.setFileDataMode(file_modes[i]));
        }
        
    }
}

