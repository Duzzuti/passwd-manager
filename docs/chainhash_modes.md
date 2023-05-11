# Chainhash modes

| CH-Mode |Data block format| Doc                                                                                                                              |
|---------|---|----------------------------------------------------------------------------------------------------------------------------------|
| 1       |0B| Performs a normal chainhash (just repeat the hahsing on the same hash)                                                           |
| 2       |0-255B S| Performs a chainhash with a constant salt (repeat the hahsing on the same hash + a given salt)                                   |
| 3       |8B SN| Performs a chainhash with a count salt (repeat the hahsing on the same hash + a incrementing number)                             |
| 4       |8B SN, 0-247B S| Performs a chainhash with a count and constant salt (repeat the hahsing on the same hash + a incrementing number + a given salt) |
| 5       |8B SN| Performs a chainhash with a quadratic count salt (repeat the hahsing on the same hash + a incrementing quadratic number)         |


## Data block format
\<length in bytes>B \<data>

### data
|data|description|
|---|---|
|S|a given salt|
|SN|the start number for the count salt|