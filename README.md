# passwd-manager
this is a simple system that keeps your passwords **secure** and **flexible**

## get ready

Todo 

## functionality
### basics
the system just encrypts a file with a password.

the data is loaded on a blockchain that is made out of data blocks.
Each block contains some bytes of data that wants to be encrypted.
This bytes are added with some hash that is derived from the password.
And this sum got a third byte block with some salt that is derived from the previous block.

The sum mod 256 for each byte is the encrypted data of this block.
To get all encrypted data, the encrypted bytes of each block are concatinated.

The length of one block depends on the length of bytes that the hash function is returning. You can use *sha256*, *sha384* and *sha512*.

### how to get the hash that is derived from the password?

there are two important measurements:
1. the time it takes to generate the hash has to be *low* for good and fast access
2. the time it takes to generate the hash has to be *high* for good security

Thats why it is important to balance this time.

TODO (how exactly does the algorithm work)

### how to get the salt
the salt is a random generated byte string that is encrypted in the file.
you need to have access to the password hash to decrypt the salt.

The salt is then used for the first block as described under *basics*.

For the second block, the salt of the previous block is added to the result
of an *hashchain* that is executed on the passwordhash.
The result is the new salt.

A *hashchain* is function that calculates the hash of this input.
This input is hashed again (you can add some salt each step).
This continues for a specific amount of iterations.

### how to decrypt

TODO



## Workflow diagrams:
### decrypt workflow
<img src="docs/decrypt.png"
     alt="Decrypt workflow"
     style="float: left; margin-right: 10px;"
 />
 ###encrypt workflow
 <img src="docs/encrypt.png"
     alt="Decrypt workflow"
     style="float: left; margin-right: 10px;"
 />
