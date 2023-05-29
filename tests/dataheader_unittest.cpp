#include "gtest/gtest.h"
#include "dataheader.h"
#include "rng.h"
#include "test_settings.cpp"
#include "chainhash_data.h"
#include "settings.h"
#include "utility.h"

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
            EXPECT_THROW(DataHeader(HModes(hash_modes[i])), std::invalid_argument);
        }else{
            EXPECT_NO_THROW(DataHeader(HModes(hash_modes[i])));
        }
    }
    //some edge cases
    EXPECT_THROW(DataHeader(HModes(255)), std::invalid_argument);
    EXPECT_THROW(DataHeader(HModes(256)), std::invalid_argument);
    EXPECT_THROW(DataHeader(HModes(257)), std::invalid_argument);
    EXPECT_THROW(DataHeader(HModes(-1)), std::invalid_argument);
    EXPECT_THROW(DataHeader(HModes(-255)), std::invalid_argument);
    EXPECT_THROW(DataHeader(HModes(-256)), std::invalid_argument);
}

TEST(DataHeaderClass, setFileDataMode){
    //testing the set file data mode from the dataheader
    //creating a list of file modes, where the first (0) and the last (MAX_FILEMODE +1) are invalid
    std::vector<unsigned char> file_modes;
    for(int i=0; i <= MAX_FILEMODE_NUMBER+1; i++){
        file_modes.push_back(i);
    }

    //testing the file modes on the setter
    for(int i=0; i < file_modes.size(); i++){
        //throw testing
        if(i == 0 || i == file_modes.size() - 1){
            //invalid file mode
            EXPECT_THROW(DataHeader(HModes(STANDARD_HASHMODE)).setFileDataMode(FModes(file_modes[i])), std::invalid_argument);
        }else{
            //valid file mode
            EXPECT_NO_THROW(DataHeader(HModes(STANDARD_HASHMODE)).setFileDataMode(FModes(file_modes[i])));
            //checking if the setter returns void
            EXPECT_EQ(typeid(void), typeid(DataHeader(HModes(STANDARD_HASHMODE)).setFileDataMode(FModes(file_modes[i]))));
        }
    }
    //some edge cases
    EXPECT_THROW(DataHeader(HModes(STANDARD_HASHMODE)).setFileDataMode(FModes(255)), std::invalid_argument);
    EXPECT_THROW(DataHeader(HModes(STANDARD_HASHMODE)).setFileDataMode(FModes(256)), std::invalid_argument);
    EXPECT_THROW(DataHeader(HModes(STANDARD_HASHMODE)).setFileDataMode(FModes(257)), std::invalid_argument);
    EXPECT_THROW(DataHeader(HModes(STANDARD_HASHMODE)).setFileDataMode(FModes(-1)), std::invalid_argument);
    EXPECT_THROW(DataHeader(HModes(STANDARD_HASHMODE)).setFileDataMode(FModes(-255)), std::invalid_argument);
    EXPECT_THROW(DataHeader(HModes(STANDARD_HASHMODE)).setFileDataMode(FModes(-256)), std::invalid_argument);
}

TEST(DataHeaderClass, setChainHash){
    //testing the chainhash setters from the dataheader
    //creating a list of chainhash modes, where the first (0) and the last (MAX_CHAINHASHMODE +1) are invalid
    std::vector<unsigned char> ch_modes;
    for(int i=0; i <= MAX_CHAINHASHMODE_NUMBER+1; i++){
        ch_modes.push_back(i);
    }
    //calculate the bytes needed for the max iteration (to get all valid iterations possible)
    unsigned char rand_bytes = std::min<unsigned char>(8, getLongLen(MAX_ITERATIONS-1));

    //testing the chainhash modes with different iterations and datablocks
    for(int i=0; i < ch_modes.size(); i++){
        for(int j=0; j < TEST_DH_ITERS; j++){
            Bytes tmp;
            tmp.setBytes(RNG::get_random_bytes(rand_bytes));    //set random iterations
            u_int64_t iters = toLong(tmp);  //sets random iters
            if(iters > MAX_ITERATIONS){
                //if the random iterations are bigger than the max iterations, set the max iterations
                iters = MAX_ITERATIONS;
            }
            //init chainhashdata with chainhash mode
            Format format{CHAINHASH_NORMAL};
            if(i == 0 || i == ch_modes.size() - 1){
                //invalid chainhash mode
                EXPECT_THROW(Format(CHModes(ch_modes[i])), std::invalid_argument);
            }else{
                format = Format{CHModes(ch_modes[i])};
            }
            ChainHashData chd{format};
            std::vector<NameLen> vnl = format.getNameLenList();
            unsigned char len = 0;  //current used datablock length
            //add random datablock data
            for(NameLen nl : vnl){
                if(nl.len != 0){
                    //got a data part with set length
                    chd.addBytes(Bytes(nl.len));
                    len += nl.len;
                }else{
                    //got a data parCHModest with * length (use the remaining bytes)
                    chd.addBytes(Bytes(255 - nl.len));
                    len = 255;
                    break;
                }
            }
            EXPECT_TRUE(chd.isCompletedFormat(format));  //check if the chainhashdata is completed

            //testing the chainhash modes on the setter
            //init dataheader with random hashmode
            HModes hash_mode = HModes(RNG::get_random_byte(1, MAX_HASHMODE_NUMBER));
            Hash* hash = HashModes::getHash(hash_mode);
            unsigned char hash_size = hash->getHashSize();
            delete hash;
            DataHeader dh{hash_mode};   //testing chainhash1
            DataHeader dh2{hash_mode};  //testing chainhash2
            if(i == 0 || i == ch_modes.size() - 1){
                //invalid chainhash mode
                EXPECT_THROW(dh.setChainHash1(CHModes(ch_modes[i]), iters, len, chd), std::invalid_argument);
                EXPECT_THROW(dh2.setChainHash2(CHModes(ch_modes[i]), iters, len, chd), std::invalid_argument);
            }else{
                //valid chainhash mode
                EXPECT_NO_THROW(dh.setChainHash1(CHModes(ch_modes[i]), iters, len, chd));
                //tests on the changed object
                EXPECT_THROW(dh.getDataHeaderParts(), std::logic_error); //dataheader is not completed
                EXPECT_EQ(0, dh.getHeaderLength()); //chainhash2 is not set, therefore no length can be computed
                EXPECT_THROW(dh.getHeaderBytes(), std::logic_error); //dataheader is not completed
                EXPECT_THROW(dh.calcHeaderBytes(Bytes(hash_size)), std::logic_error); //dataheader is not completed

                //chainhash2
                //valid chainhash mode
                EXPECT_NO_THROW(dh2.setChainHash2(CHModes(ch_modes[i]), iters, len, chd));
                //tests on the changed object
                EXPECT_THROW(dh2.getDataHeaderParts(), std::logic_error); //dataheader is not completed
                EXPECT_EQ(0, dh2.getHeaderLength()); //chainhash1 is not set, therefore no length can be computed
                EXPECT_THROW(dh2.getHeaderBytes(), std::logic_error); //dataheader is not completed
                EXPECT_THROW(dh2.calcHeaderBytes(Bytes(hash_size)), std::logic_error); //dataheader is not completed

                //setting both chainhashes
                EXPECT_NO_THROW(dh.setChainHash2(CHModes(ch_modes[i]), iters, len, chd));
                EXPECT_NO_THROW(dh2.setChainHash1(CHModes(ch_modes[i]), iters, len, chd));
                //tests on the changed object
                EXPECT_THROW(dh2.getDataHeaderParts(), std::logic_error); //dataheader is not completed
                EXPECT_EQ(22 + 2*hash_size + 2*chd.getLen(), dh2.getHeaderLength()); //both chainhashes are set, therefore a length can be computed
                EXPECT_THROW(dh2.getHeaderBytes(), std::logic_error); //dataheader is not completed
                EXPECT_THROW(dh2.calcHeaderBytes(Bytes(hash_size)), std::logic_error); //dataheader is not completed
            }
            //some edge cases
            EXPECT_THROW(DataHeader(HModes(STANDARD_HASHMODE)).setChainHash1(CHModes(ch_modes[i]-255), iters, len, chd), std::invalid_argument);
            EXPECT_THROW(DataHeader(HModes(STANDARD_HASHMODE)).setChainHash1(CHModes(ch_modes[i]-256), iters, len, chd), std::invalid_argument);
            EXPECT_THROW(DataHeader(HModes(STANDARD_HASHMODE)).setChainHash1(CHModes(0), iters, len, chd), std::invalid_argument);
            EXPECT_THROW(DataHeader(HModes(STANDARD_HASHMODE)).setChainHash1(CHModes(-1), iters, len, chd), std::invalid_argument);
            EXPECT_THROW(DataHeader(HModes(STANDARD_HASHMODE)).setChainHash2(CHModes(ch_modes[i]-255), iters, len, chd), std::invalid_argument);
            EXPECT_THROW(DataHeader(HModes(STANDARD_HASHMODE)).setChainHash2(CHModes(ch_modes[i]-256), iters, len, chd), std::invalid_argument);
            EXPECT_THROW(DataHeader(HModes(STANDARD_HASHMODE)).setChainHash2(CHModes(0), iters, len, chd), std::invalid_argument);
            EXPECT_THROW(DataHeader(HModes(STANDARD_HASHMODE)).setChainHash2(CHModes(-1), iters, len, chd), std::invalid_argument);
        }
    }
}
