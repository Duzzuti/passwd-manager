# API CLASS WORKFLOWS
## Workflow1 (for old files):
1. **get the path to the .enc file that you wanna work with**

    *the api has some functionality for these actions*
    1. get the current working directory (`getCurrentDirPath()`)
    1. get the default directory where the .enc files are stored (in the user's home directory) (`getEncDirPath()`)
    1. get the names of all .enc files in a given directory (`getEncFileNames()`)
<br/><br/>
1. **create the `API` object**

1. **get the content of the .enc file**
    1. get the complete content of a .enc file at a given path (`getFileContent()`)
    1. get the data header from the content you got from the .enc file (`getDataHeader()`)
    1. get the encrypted data from the content you got from the .enc file (`getData()`)
<br/><br/>
1. **the user provides the password to decrypt the file**
    1. check the password with `verifyPassword()` that will also save the correct password hash 
    (to prevent calculating it again)
<br/><br/>
1. **decrypt the data** 
    1. decrypt the data with `getDecryptedData(enc_data)`
    1. get the `FileData` object with `getFileData(dec_data from 5.1.)`
<br/><br/>
1. **work with the `FileData` object**
    1. `FileData` objects have methods to get the data in a special format or to modify the data
    1. the actual object depends on which data is stored in the file
    1. check the documentation of the `FileData` objects for more information
<br/><br/>
1. **[OPTIONAL] create new dataheader (change the encryption)**
    1. create a new dataheader with `createDataHeader()`
    
    you can get the settings object from the old dataheader (by calling DataHeader::getSettings())

    NOTE that this new dataheader + password will be used in `8.encrypt data`
    
    **THE OLD PASSWORD IS NOT LONGER VALID**

1. **encrypt the data**
    1. get the decrypted data from the file data object with `getDecryptedData(file_data)`
    1. encrypt the data with `getEncryptedData(dec_data from 7.1.)`
<br/><br/>
1. **write to file**
    1. check if the given file matches with the file which was read in `getFileContent()`, an empty file is also approved
    1. if not that means that the user is trying to overwrite an other file. 
        1. add files unsafe via `addFile()`. **THE USER SHOULD CONFIRM THIS ACTION** (because it will overwrite an other file)
        1. `checkFile()` has to be called first with a file-not-approved result in order to call `addFile()`
        1. dont call `addFile()` if the file is valid
    1. save the encrypted data to the file with `writeToFile(filePath, enc_data)`
<br/><br/>
1. call `logout()` to delete user informations from this `API` object (does the same as destructor + constructor again)



## Workflow2 (for new files):
1. **get the path to the new .enc file that you wanna work with**

    *the api has some functionality for these actions*
    1. get the current working directory (`getCurrentDirPath()`)
    1. get the default directory where the .enc files are stored (in the user's home directory) (`getEncDirPath()`)
    1. create a new .enc file at a given path (`createEncFile()`)
<br/><br/>
1. **create the API object**

1. **create a new dataheader for this file**
    1. get a master password that should be set for this file from the user
    1. create a dataheader settings object 
        1. With `DataHeaderSettingsIters` you can choose a number of iterations for the chainhash

        That means this could run very long (you can provide a timeout)
        1. With `DataHeaderSettingsTime` you can choose calculation times for the chainhash 
        
        (it will take the iteration count that is reached if the time is over)
    1. create a new dataheader with `createDataHeader(master_password, dataheader settings)`

1. **get a new file data object**
    1. get the file data object with `getNewFileData()`
<br/><br/>
1. **work with the `FileData` object**
    1. `FileData` objects have methods to get the data in a special format or to modify the data
    1. the actual object depends on which data is stored in the file
    1. check the documentation of the `FileData` objects for more information
<br/><br/>
1. **encrypt the data**
    1. get the decrypted data from the file data object with `getDecryptedData(file_data)`
    1. encrypt the data with `getEncryptedData(dec_data from 7.1.)`
<br/><br/>
1. **write to file**
    1. check if the given file matches with the file which was read in `getFileContent()`, an empty file is also approved
    1. if not that means that the user is trying to overwrite an other file. 
        1. add files unsafe via `addFile()`. **THE USER SHOULD CONFIRM THIS ACTION** (because it will overwrite an other file)
        1. `checkFile()` has to be called first with a file-not-approved result in order to call `addFile()`
        1. dont call `addFile()` if the file is valid
    1. save the encrypted data to the file with `writeToFile(filePath, enc_data)`
<br/><br/>
1. call `logout()` to delete user informations from this `API` object (does the same as destructor + constructor again)

## Workflow3 (delete a file)
1. **get the path to the .enc file that you wanna delete**

    *the api has some functionality for these actions*
    1. get the current working directory (`getCurrentDirPath()`)
    1. get the default directory where the .enc files are stored (in the user's home directory) (`getEncDirPath()`)
    1. get the names of all .enc files in a given directory (`getEncFileNames()`)
<br/><br/>
1. **delete the file**

    **THE USER SHOULD CONFIRM THIS ACTION**
    1. with `deleteEncFile()`

