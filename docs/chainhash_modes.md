# Chainhash modes

| CH-Mode |Data block format| Doc                                                                                                                              |
|---------|---|----------------------------------------------------------------------------------------------------------------------------------|
| 1       || Performs a normal chainhash (just repeat the hashing on the same hash)                                                           |
| 2       |*B S| Performs a chainhash with a constant salt (repeat the hashing on the same hash + a given salt)                                   |
| 3       |8B SN| Performs a chainhash with a count salt (repeat the hashing on the same hash + a incrementing number)                             |
| 4       |8B SN, *B S| Performs a chainhash with a count and constant salt (repeat the hashing on the same hash + a incrementing number + a given salt) |
| 5       |8B SN 8B A 8B B 8B C| Performs a chainhash with a quadratic count salt (repeat the hashing on the same hash + a incrementing quadratic number)         |


## Data block format
\<length in bytes>B \<data_name>

The maximum length of one data block is 255 Bytes.

You can use a `*B <data_name>` to allow a variable length. But you can only use one `*B` per block and only at the end. 


### data
|data_name|description|
|---|---|
|S|a given salt|
|SN|the start number for the count salt|
|A, B, C|8 Byte number arguments|