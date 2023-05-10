# DataHeader
|Bytes|Type|Doc|More Docs|
|---|---|-------------|-----|
|1|unsigned char|which hash function was used in this file|hash_modes.md|
|1|unsigned char|which chainhash was used to get the passwordhash|chainhash_modes.md|
|8|long|saves the number of iterations for turning the password into the passwordhash|bytes.md|
|1|int|saves the length (in bytes) of the datablock for the first chainhash|chainhash_modes.md|
|0-255|Bytes|data block for the first chainhash|chainhash_modes.md|
|1|unsigned char|which chainhash was used to validate the passwordhash|chainhash_modes.md|
|8|long|saves the number of iterations for the second chainhash (password validating)|bytes.md|
|1|int|saves the length (in bytes) of the datablock for the second chainhash|chainhash_modes.md|
|0-255|Bytes|data block for the second chainhash|chainhash_modes.md|
|Hash size|Bytes|saves the bytes of a chainhash from the passwordhash to validate the password|-|
|Hash size|Bytes|saves the encrypted salt|doc.md|


### Total length of the data header lh:
    21 + 2\*HS <= lh <= 21 + 2\*HS + 2\*255 Bytes

|Hash size|Min lh|Max lh|
|---|---|---|
|32|85|595|
|48|117|627|
|64|149|659|