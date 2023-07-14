# Password data
## Related
### Docs
- [File data](/docs/file_data.md)
### Classes
- [PasswordData](/include/password_data.h)
## How to use the password data class
// WORK
## How the data is stored
|Bytes|Type|Doc|More Docs|
|---|---|-------------|-----|
|1|unsigned char|saves the length (in bytes) of the site name block|-|
|1-255|string|name of the site|-|
|1|unsigned char|saves the length (in bytes) of the username|-|
|1-255|string|username|-|
|1|unsigned char|saves the length (in bytes) of the email|-|
|1-255|string|email|-|
|1|unsigned char|saves the length (in bytes) of the password|-|
|1-255|string|password|-|