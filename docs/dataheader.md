# DataHeader
|Bytes|Modes|Type|Doc|More Docs|
|----|----|----|-------------|-----|
|1|ALL|unsigned char|saves the mode, in which the file is encrypted|modes.md|
|32<br>48<br>64|1,4<br>2,5<br>3,6|Bytes|saves the bytes of a chainhash from the password hash to make sure the password the user entered was right|-|
|8|ALL|long|saves the number of iterations for turning the passwordhash into another hash for password checking|bytes.md|
|1|ALL|unsigned char|which chainhash was used for the password checking hash|TODO Modes of chainhashs|
|32<br>48<br>64|1,4<br>2,5<br>3,6|Bytes|saves the encrypted salt|-|
|8|4-6|long|saves the number of iterations for turning the password into the passwordhash for decrypting the salt|bytes.md|

|Mode|Full byte length|Parts|
|---|---|---|
|1|74|1+32+8+1+32|
|2|106|1+48+8+1+48|
|3|138|1+64+8+1+64|
|4|82|1+32+8+1+32+8|
|5|114|1+48+8+1+48+8|
|6|146|1+64+8+1+64+8|