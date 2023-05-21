## More Docs
For information about the header length for each mode: *dataheader.md*

## Explanations
### Salt
A random generated salt is saved in the data header. It is encrypted, that means we have to decrypt it with the password. The password has to get chainhashed to an passwordhash.

We have to subtract the passwordhash from the encrypted salt to decrypt it (bytewise mod 256)

### Chainhash
A chainhash is a function that is iterating a given number of iterations. In each iteration it performs a hash on the data. The result is stored in this data again.

this makes it difficult for attackers who have even less chance to crack the hash. (Only brute forcing is left)

### Security
For security reason, it is **not recommended** to use simple hashes for the password. 

Security depends highly on the iteration number. If you use a strong chainhash AND a high number of iterations, it will be secure to use.

The security is based on the fact that you cannot perform a inverse hash.

The higher the iteration count the safer the chainhash.

Please note, that security is always leading to longer decryption time.