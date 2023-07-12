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
|1|unsigned char|which kind of data is in the file|[*file_data.md*](file_data.md)|
|1|unsigned char|which hash function was used in this file|[*hash_modes.md*](hash_modes.md)|
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


### Total length of the data header lh:
    22 + 2*HS <= lh <= 22 + 2*HS + 2*255 Bytes

|Hash size|Min lh|Max lh|
|---|---|---|
|32|86|596|
|48|118|628|
|64|150|660|

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
//WORK