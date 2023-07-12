# DataHeader
## Related
### Docs
- [General](/docs/doc.md)
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
//WORK

## How does the data header work?
//WORK