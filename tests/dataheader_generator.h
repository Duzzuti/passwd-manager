#include "dataheader.h"
#include "rng.h"

struct DataHeaderGenSet{
    std::optional<HModes> hashmode;
    std::optional<unsigned char> datablocknum;
    std::optional<unsigned char> decdatablocknum;
    std::optional<unsigned char> chainhashlen1;
    std::optional<unsigned char> chainhashlen2;
};

class DataHeaderGen{
private:
    static unsigned char getChainHashMode(unsigned char datablocklen){
        unsigned char cm = -1;
        while(true){
            cm = RNG::get_random_byte(1, MAX_CHAINHASHMODE_NUMBER);  // chain hash mode
            if(datablocklen == 0){
                if(!(cm == CHAINHASH_NORMAL || cm == CHAINHASH_CONSTANT_SALT))
                    continue;
                else 
                    break;
            }else if(datablocklen < 8)
                cm = CHAINHASH_CONSTANT_SALT;
            if(datablocklen != 8 && cm == CHAINHASH_COUNT_SALT)
                continue;
            if(datablocklen != 32 && cm == CHAINHASH_QUADRATIC)
                continue;
            if(datablocklen != 0 && cm == CHAINHASH_NORMAL)
                continue;
            break;
        }
        return cm;
    }
public:
    static Bytes generateDH(DataHeaderGenSet set = DataHeaderGenSet()){
        Bytes ret(16); // length of file and header
        ret.fillrandom(); // set in the end
        ret.addSize(3);
        ret.addByte(1); // file mode
        unsigned char hashmode = set.hashmode.has_value() ? set.hashmode.value() : RNG::get_random_byte(1, MAX_HASHMODE_NUMBER); // hash mode
        ret.addByte(hashmode);
        unsigned char datablocknum = set.datablocknum.has_value() ? set.datablocknum.value() : RNG::get_random_byte();
        ret.addByte(datablocknum); // number of datablocks
        for(u_int16_t i = 0; i < datablocknum; i++){
            unsigned char len = RNG::get_random_byte(1);
            ret.addSize(len + 2);
            ret.addByte(RNG::get_random_byte()); // datablock type
            ret.addByte(len); // datablock length
            Bytes tmp(len);
            tmp.fillrandom();
            tmp.addcopyToBytes(ret);
        }
        ret.addSize(9);
        unsigned char clen1 = set.chainhashlen1.has_value() ? set.chainhashlen1.value() : RNG::get_random_byte();
        ret.addByte(DataHeaderGen::getChainHashMode(clen1));   // chain hash mode
        for(int i = 0; i < 5; i++)
            ret.addByte(0);
        ret.fillrandom();   //iterations
        Bytes tmp2(clen1);
        tmp2.fillrandom();
        ret.addSize(clen1 + 10);
        ret.addByte(clen1);
        tmp2.addcopyToBytes(ret);

        unsigned char clen2 = set.chainhashlen2.has_value() ? set.chainhashlen2.value() : RNG::get_random_byte();
        ret.addByte(DataHeaderGen::getChainHashMode(clen2));   // chain hash mode
        for(int i = 0; i < 5; i++)
            ret.addByte(0);
        ret.fillrandom();   //iterations

        Bytes tmp3(clen2);
        tmp3.fillrandom();
        ret.addSize(clen2 + 1 + 1 + 2*HashModes::getHash(HModes(hashmode))->getHashSize());
        ret.addByte(clen2);
        tmp3.addcopyToBytes(ret);
        ret.addrandom(ret.getMaxLen() - ret.getLen() - 1);
        unsigned char ddatablocknum = set.decdatablocknum.has_value() ? set.decdatablocknum.value() : RNG::get_random_byte();
        ret.addByte(ddatablocknum); // number of dec datablocks
        for(u_int16_t i = 0; i < ddatablocknum; i++){
            ret.addSize(257);
            ret.addByte(RNG::get_random_byte()); // enc datablock type
            ret.addByte(RNG::get_random_byte()); // enc datablock length
            Bytes tmp(255);
            tmp.fillrandom();
            tmp.addcopyToBytes(ret);
        }
        Bytes len = Bytes::fromLong(ret.getLen(), true);
        std::memcpy(ret.getBytes(), len.getBytes(), len.getLen());
        std::memcpy(ret.getBytes() + 8, len.getBytes(), len.getLen());
        return ret;
    }
};