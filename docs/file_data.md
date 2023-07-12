# File data modes
## Related
### Docs
- [API](/docs/api.md)
- [Password data](/docs/password_data.md)
### Classes
- [FileData](/include/file_data.h)<br>
- [FileMode](/include/file_modes.h)
### Enums
- [FModes](/include/base.h)<br>
### Structs
- [FileDataStruct](/include/file_data.h)
## Why we need different modes
To take actions on the decrypted data, we need to know which kind of data is stored in the file.

The user could have stored passwords in the file, plain txt or image data. In each of this cases we need to ask the user different questions.

If the user stores the passwords, the user may be asked about the site name from which a password should be returned.
But the same does not apply to images or other kind of data.

The data is stored in **sets**. One set has a format and represents one piece of data (e.g. one password or one line of text)

## Where did i use the file data mode?
### -> to handle the decrypted data
1. you need a `FileDataStruct`
    - you can get it from the `getDecryptedData()` or from `getFileData()` from the [API](api.md#working-with-the-api) object
    - you also can create it by you own with a `FileMode` and some decrypted (or empty) data
2. construct the `FileData` object (for example [PasswordData](password_data.md)) with the `FileDataStruct`
3. now you can work with the `FileData` object to change internal data or to get the data in a special format
4. in the end you can get the new `FileDataStruct` from the `FileData` object with `getFileData()`
5. you can use this `FileDataStruct` to encrypt the data and save it to the file with [API](api.md#working-with-the-api) methods

## File data modes list
|File Mode|Stores|Data format per set|
|---|---|---|
|1|Passwords|[*password_data.md*](password_data.md#how-the-data-is-stored)|