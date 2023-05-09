# Bytes

## Byte operations

### Turning Bytes to Numbers
``` c++
    long ret = 0;
    std::vector<unsigned char> v = b.getBytes();
    for(int i=0; i < v.size(); i++){
        long byte_value = toLong(v[i]);
        long byte_position = std::pow(256, v.size()-i-1);
        ret += byte_value*byte_position;
    }
```
this method is implemented in bytes/toLong()

First it is looping over all bytes.<br>For each byte we will take its value (from 0 - 255) and also the position of the byte in the whole bytestring.

Due to the fact that we iterate over the highest byte first, the byte position has to be calculated first.

The position is the <br>length of the bytestring - the current index position of the byte - 1 <br>(-1 because we count from zero)

The postion of the byte is the exponent for the byte`s position weight.
<br>(We need to take 256^byte_pos, because we are in a number system with base 256 (one byte))

Now we have to add all of the weighted byte values to get the value for the full string.