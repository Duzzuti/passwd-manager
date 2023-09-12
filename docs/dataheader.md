# DataHeader
## Related
### Docs
- [General](/docs/doc.md)
- [API](/docs/api.md)
- [Bytes](/docs/bytes.md)
- [File data](/docs/file_data.md)
- [Hash modes](/docs/hash_modes.md)
- [Chainhash modes](/docs/chainhash_modes.md)
### Classes
- [DataHeader](/include/dataheader.h)
### Structs
- [DataHeaderParts](/include/dataheader.h)
- [DataHeaderSettingsIters](/include/dataheader.h)
- [DataHeaderSettingsTime](/include/dataheader.h)
## DataHeader Bytes
//WORK correct link of the docs
|Bytes|Type|Doc|More Docs|
|---|---|-------------|-----|
|8|u_int64|how long is the whole file in Bytes|-|
|8|u_int64|how long is the dataheader in Bytes|-|
|1|unsigned char|which kind of data is in the file|[*file_data.md*](file_data.md)|
|1|unsigned char|which hash function was used in this file|[*hash_modes.md*](hash_modes.md)|
|1|unsigned char|how many datablocks are following|WORK|
|||||
|||DATABLOCKS||
|1|unsigned char|which type of data is stored in that block|WORK|
|1|unsigned char|how long is the datablock in Bytes|WORK|
|0-255|Bytes|data of the datablock|WORK|
|||||
|1|unsigned char|which chainhash was used to get the passwordhash|- [*chainhash_modes.md*](chainhash_modes.md)|
|8|u_int64|saves the number of iterations for turning the password into the passwordhash|[*bytes.md*](bytes.md)|
|1|unsigned char|saves the length (in bytes) of the datablock for the first chainhash|- [*chainhash_modes.md*](chainhash_modes.md)|
|0-255|Bytes|data block for the first chainhash|- [*chainhash_modes.md*](chainhash_modes.md)|
|1|unsigned char|which chainhash was used to validate the passwordhash|- [*chainhash_modes.md*](chainhash_modes.md)|
|8|u_int64|saves the number of iterations for the second chainhash (password validating)|[*bytes.md*](bytes.md)|
|1|unsigned char|saves the length (in bytes) of the datablock for the second chainhash|- [*chainhash_modes.md*](chainhash_modes.md)|
|0-255|Bytes|data block for the second chainhash|- [*chainhash_modes.md*](chainhash_modes.md)|
|Hash size|Bytes|saves the bytes of a chainhash from the passwordhash to validate the password|-|
|Hash size|Bytes|saves the encrypted salt|[*doc.md*](doc.md)|
|1|unsigned char|how many decrypted datablocks are following|WORK|
|||||
|||DECRYPTED DATABLOCKS||
|1|unsigned char|which type of data is stored in that block (encrypted)|WORK|
|1|unsigned char|how long is the datablock in Bytes|WORK|
|0-255|Bytes|data of the datablock (encrypted)|WORK|
|||||
|||||

### Total length of the data header lh:
    lh = 40 + 2*HashSize + cs + d + ed
    with: cs = cs1 + cs2 < 2*255 Bytes (chainhash-datablocks)
          d = nd*2 + sdd    (datablocks)
          ed = ned*2 + sedd (encrypted datablocks)

    => lh = 40 + 2*HashSize + cs1 + cs2 + 2(nd+ned) + sdd + sedd

    and:  cs1, cs2 = chainhash datablock sizes
          nd = number of datablocks
          sdd = sum of all data from datablocks
          ned = number of encrypted datablocks
          sedd = sum of all data from encryped datablocks


|Hash size|Min lh|Max lh (without datablocks)|
|---|---|---|
|32|104|614|
|48|136|646|
|64|168|678|

each datablock is adding between 2 and 257 Bytes on data.<br>
There are 510 (2*255) datablocks maximum.
That means that the maximum dataheader size is:

|Hash size|Min lh|Max lh|
|---|---|---|
|32|104|131684 (132KB)|
|48|136|131716 (132KB)|
|64|168|131748 (132KB)|

## DataHeaderSettings
`DataHeaderSettings` are used in the `API` to create a new `DataHeader`. These structs contain all information that is needed to generate a new `DataHeader`.

The following settings are available:
- `file_mode` that defines which kind of data is stored in the file
    - has to match with the `API`s file mode
    - docs: [file_modes](file_data.md)
- `hash_mode` that defines which hash function is used for the encryption
    - docs: [hash_modes](hash_modes.md)
- `chainhash_modes` which define the kind of chainhashes that are used to encrypt the password
    - the first chainhash is used to encrypt the password
    - the second chainhash is used to validate the password
    - docs: [chainhash_modes](chainhash_modes.md)

There are two different `DataHeaderSettings` structs:
- `DataHeaderSettingsIters` that defines the number of iterations for the chainhashes
    - you can specify how many iterations the chainhashes should have with `chainhash1_iters` and `chainhash2_iters`
    - note that on slow machines or with a high number of iterations the encryption can take a very long time
    - thats why there is often a `timeout` parameter that defines after how many milliseconds the encryption should be aborted (e.g. in the `API::createDataHeader` method)<br><br>
- `DataHeaderSettingsTime` that defines the time that the chainhashes takes
    - you can specify how long the chainhashes should take with `chainhash1_time` and `chainhash2_time` (in milliseconds)
    - the encryption will be run until the time is reached, the dataheader will be created with the number of iterations that were reached in the time
    - that means that the encryption will take exactly the time that you specified
    - note that if you generating the dataheader on a fast machine and running the encryption on a slow machines afterwards it will take longer

the remaining parts of the dataheader are generated automatically:
- `chainhash1_datablock` and `chainhash2_datablock` are generated randomly
- `chainhash1_datablock_size` and `chainhash2_datablock_size` are set to the size of the datablocks
- `enc_salt` is generated randomly with the hash size of the hash function

after that the dataheader is generated with the `DataHeader::setHeaderParts()` method internally
## How does the data header work?
The data header is used to store all information that has to be known in order to decrypt the file. It is stored at the beginning of the file. The header contains the following information (see [above](#dataheader-bytes) for a more ordered list with links to the docs):
- `file_mode`, `hash_mode`
- `chainhash1_mode`, `chainhash2_mode`
- `chainhash1_iters`, `chainhash2_iters`
- `chainhash1_datablock_size`, `chainhash2_datablock_size`
- `chainhash1_datablock`, `chainhash2_datablock`
- `enc_salt`
- `password validation hash`

The `file_mode` is used to check if the file contains the right kind of data. The API will deny a file if the file mode does not match with the API's file mode. The `file_mode` is also used to create the right `FileData` object (see [FileData](file_data.md#why-we-need-different-modes)).

Due to the `hash_mode` the API knows which hash function was used to encrypt the data. Different hash functions could be added in the future. The `hash_mode` is used to create the right `Hash` object (see [Hash](hash_modes.md#how-does-hash-modes-work)).

The `chainhash1_mode` is another mode that defines how the first chainhash should behave.

The `chainhash1_iters` is an 8 Byte value that defines how many iterations the first chainhash should have.

The `chainhash1_datablock_size` is a 1 Byte value that defines how long (in Bytes) the datablock for the first chainhash should be.

The `chainhash1_datablock` is a `Bytes` object that contains the actual datablock for the first chainhash. That means all data, like salts etc. that are used in that chainhash.

`chainhash2_*` elements are analog to chainhash1 elements, but for the second chainhash (that chainhash that validates the password).
- see [Chainhash](chainhash_modes.md#how-does-chainhash-modes-work) for more information about chainhashes

The `enc_salt` is a `Bytes` object that contains the encrypted salt. It is exactly as long as the hash function's hash size. It is used to make the encryption more secure:
- the `enc_salt` has to be decrypted with the passwordhash (XOR)
- the gotten salt is used with the passwordhash to encrypt the actual data
- the salt can (and should) be generated randomly
- for more information see [Encryption](doc.md)//WORK

The `password validation hash` is a `Bytes` object that contains the hash of the passwordhash. It is exactly as long as the hash function's hash size. It is used to validate the password:
- the password is encrypted with the first chainhash
- the result is encrypted with the second chainhash
- the result is compared with the `password validation hash`
- if the hashes match, the password is correct

A potential attacker can not get the password from the `password validation hash` because the hash function is not reversible. The attacker can only try to guess the password and encrypt it with the chainhashes (brute force).