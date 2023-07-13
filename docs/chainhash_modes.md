# Chainhash modes
## Usage
The chainhash modes decide the chainhash to use to encrypt the password and validate it. The chainhash is a hash function that is repeated multiple times. The different modes allow you to use different salts and different count values to make the chainhash more secure.

In one data header you can use two different chainhashes. The first one is used to encrypt the password to a passwordhash.
This passwordhash is used for encryption and decryption. The second chainhash is used to validate the password. The passwordhash is encrypted with the second chainhash and the result is compared with the passwordhash from the data header. If they are the same, the password is correct.

We do that to decide whether the entered password is correct. If we would not validate the password, we would not know if the entered password is correct or if the decryption failed.

## Datablock
the 5 different chainhash modes use different datablocks. The datablock is a part of the data header that is used by the chainhash to add salts etc.
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