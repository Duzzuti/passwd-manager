# Hash Modes
|Mode|Secure(1-10)|Doc|
|---|---|---|
|1|<span style="color:red">1</span>|takes the password and performs one sha256 on it|
|2|<span style="color:red">1</span>|takes the password and performs one sha384 on it|
|3|<span style="color:red">1</span>|takes the password and performs one sha512 on it|
|4|<span style="color:orange">1-8</span> (depends on iterations)|takes the password and performs a simple sha256 chainhash on it|
|5|<span style="color:orange">1-8</span> (depends on iterations)|takes the password and performs a simple sha384 chainhash on it|
|6|<span style="color:orange">1-8</span> (depends on iterations)|takes the password and performs a simple sha512 chainhash on it|

<br>

## More Docs
For information about the header length for each mode: *dataheader.md*

## Explanations
### Chainhash
A chainhash is a function that is iterating a given number of iterations. In each iteration it performs a hash on the data. The result is stored in this data again.

this makes it difficult for attackers who have even less chance to crack the hash. (Only brute forcing is left)

### Security
For security reason, it is **not recommended** to use modes 1-3. 

Security depends highly on the iteration number. If you use a strong chainhash AND a high number of iterations, it will be secure to use.

The security is based on the fact that you cannot perform a inverse hash.

The higher the iteration count the safer the chainhash.

Please note, that security is always leading to longer decryption time.