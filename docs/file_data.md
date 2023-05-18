# File data modes

To take actions on the decrypted data, we need to know which kind of data is stored in the file.

The user could have stored passwords in the file, plain txt or image data. In each of this cases we need to ask the user different questions.

If the user stores the passwords, the user may be asked about the site name from which a password should be returned.
But the same does not apply to images or other kind of data.

The data is stored in **sets**. One set has a format and represents one piece of data (i.e. one password or one line of text)

|File Mode|Stores|Data format per set|
|---|---|---|
|1|Passwords|*password_data.md*|