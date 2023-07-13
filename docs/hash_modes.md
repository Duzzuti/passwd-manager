# Hash modes
## Related
### Docs
- [Bytes](/docs/bytes.md)
- [DataHeader](/docs/dataheader.md)
- [Chainhash](/docs/chainhash_modes.md)
- [BlockChain](/docs/blockchain.md)
### Classes
- [Hash](/include/hash.h)
- [HashModes](/include/hash_mode.h)
- [Sha256](/include/sha256.h)
- [Sha384](/include/sha384.h)
- [Sha512](/include/sha512.h)

## Available hash modes
|Mode|Security|Hash function|Hash size|
|---|---|---|---|
|1|<span style="color:green">safe</span>|sha256|32|
|2|<span style="color:green">safe</span>|sha384|48|
|3|<span style="color:green">safe</span>|sha512|64|

## How does hash modes work?
The hash modes define the hash function that is used.
The hash function is used for encryption as a [`chainhash`](chainhash_modes.md#usage).
There are 3 main usages of the hash function:
- encrypt the password to a passwordhash
- validate the passwordhash by encrypting it further
- encrypt the actual data by using the passwordhash and a salt as well as some more encryption to build up the [`blockchain`](blockchain.md)//WORK

All hash modes refer to one hash function object that derives from the `Hash` base class.
You can get the hash function object with the `HashModes::get_hash()` function.

## Hash object
The hash object is a class that derives from the `Hash` base class and is responsible for the actual hash function.
On every hash object there are 2 methods:
- `hash()` that hashes a given `Bytes` or `String` object (overloaded)
- `getHashSize()` that returns the size of the hash in bytes

The result of the hash function is always a [`Bytes`](bytes.md)//WORK object.