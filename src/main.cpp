#include <iostream>

#include "api.h"
#include "logger.h"

// temp
#include "password_data.h"
#include "rng.h"
#include "timer.h"
#include "utility.h"

int main(int argc, char* argv[]) {
    // for (int i = 0; i < argc; i++){
    //     std::cout << "a"<<argv[i]<<"b" << std::endl;
    //     if (argv[i] == "-f"){
    //         if (argc == i){
    //             std::cout << "break";
    //             return 1;
    //         }
    //         std::cout << "no break";
    //         i++;
    //         char* path = argv[i];
    //         FileHandler FH;
    //         std::cout << FH.setFilePath(path);
    //         continue;
    //     }
    // }

    // init logger
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    // add file logger
    static plog::RollingFileAppender<plog::TxtFormatter> fileAppender("data/log.txt", 1024 * 1024 * 10, 3);
    plog::init(plog::verbose, &consoleAppender).addAppender(&fileAppender);

    // test constants
    // std::filesystem::path FILE =  charVecToString(RNG::get_random_bytes(5)) + ".enc";
    std::filesystem::path FILE = "qqY�7.enc";
    std::string PASS = "test";
    int sleepTime = 5;

    DataHeaderSettingsIters dsi;
    {
        dsi.setChainHash1Mode(CHAINHASH_CONSTANT_COUNT_SALT);
        dsi.setChainHash2Mode(CHAINHASH_QUADRATIC);
        dsi.setChainHash1Iters(1000000);
        dsi.setChainHash2Iters(200000);
        dsi.setFileDataMode(FILEMODE_PASSWORD);
        dsi.setHashMode(HASHMODE_SHA256);
    }
    Timer timer;
    timer.start();
    if (0) {
        enum ACTION { ADD, SELECT, DELETE, UNSELECT, VERPASS, CREATEDH };
        ACTION ACT1 = ADD;
        ACTION ACT2 = CREATEDH;
        ACTION ACT3 = CREATEDH;

        PLOG_INFO << "Starting application";

        API api{FModes::FILEMODE_PASSWORD};
        PLOG_DEBUG << "API created";
        std::filesystem::path dir = api.getEncDirPath().returnValue();
        PLOG_DEBUG << "Getting enc dir path: " << dir;
        ErrorStruct<std::vector<std::string>> paths = api.getAllEncFileNames(dir);
        PLOG_DEBUG << "Getting all enc file names: " << paths.returnValue().size();
        for (auto& path : paths.returnValue()) {
            PLOG_DEBUG << path;
        }

        // INIT
        ErrorStruct<bool> err;
        switch (ACT1) {
            case ADD:
                PLOG_DEBUG << "Adding file";
                err = api.createFile(dir / FILE);
                if (err.isSuccess())
                    PLOG_DEBUG << "Added file successfully";
                else
                    PLOG_WARNING << "Failed to add file: " << getErrorMessage(err);
                PLOG_DEBUG << "Selecting file";
                err = api.selectFile(dir / FILE);
                if (err.isSuccess())
                    PLOG_DEBUG << "Selected file successfully";
                else
                    PLOG_WARNING << "Failed to select file: " << getErrorMessage(err);
                break;

            case SELECT:
                PLOG_DEBUG << "Selecting file";
                err = api.selectFile(dir / FILE);
                if (err.isSuccess())
                    PLOG_DEBUG << "Selected file successfully";
                else
                    PLOG_WARNING << "Failed to select file: " << getErrorMessage(err);
                break;
        }
        // sleep(sleepTime);
        // FILE_SELECTED
        Bytes phash;
        DataHeader dh{HASHMODE_SHA256};
        ErrorStruct<Bytes> temp;

        switch (ACT2) {
            case DELETE:
                PLOG_DEBUG << "Deleting file";
                err = api.deleteFile();
                if (err.isSuccess())
                    PLOG_DEBUG << "Deleted file successfully";
                else
                    PLOG_WARNING << "Failed to delete file: " << getErrorMessage(err);
                break;

            case UNSELECT:
                PLOG_DEBUG << "Unselecting file";
                err = api.unselectFile();
                if (err.isSuccess())
                    PLOG_DEBUG << "Unselected file successfully";
                else
                    PLOG_WARNING << "Failed to unselect file: " << getErrorMessage(err);
                break;

            case VERPASS:
                PLOG_DEBUG << "Verifying password";
                temp = api.verifyPassword(PASS);
                if (temp.isSuccess()) {
                    PLOG_DEBUG << "Verified password successfully";
                    phash = temp.returnValue();
                    PLOG_DEBUG << "Password hash: " << toHex(phash);
                } else
                    PLOG_WARNING << "Failed to verify password: " << getErrorMessage(temp);
                break;

            case CREATEDH:
                PLOG_DEBUG << "Creating data header";
                ErrorStruct<DataHeader> temp = api.createDataHeader(PASS, dsi);
                if (temp.isSuccess()) {
                    PLOG_DEBUG << "Created data header successfully";
                    dh = temp.returnValue();
                    PLOG_DEBUG << "DataHeader: " << dh.getDataHeaderParts() << "\n Hex: " << toHex(dh.getHeaderBytes());
                } else
                    PLOG_WARNING << "Failed to create data header";
                break;
        }

        // DECRYPTED
        FileDataStruct fds = api.getFileData().returnValue();
        PLOG_INFO << "Constructing file data: " << toHex(fds.dec_data);
        // PasswordData pd;
        // if(!pd.constructFileData(fds).isSuccess())
        //     PLOG_ERROR << "Failed to construct password data";
        // else
        //     PLOG_DEBUG << "Constructed password data successfully";

        Bytes b;
        b.setBytes(std::vector<unsigned char>{0x00, 0x01, 0x02, 0x03, 0x04});
        fds.dec_data = b;
        // change salt
        PLOG_DEBUG << "Changing salt";
        api.changeSalt();
        ErrorStruct<Bytes> enc_err = api.getEncryptedData(fds);
        if (enc_err.isSuccess())
            PLOG_DEBUG << "Encrypted data successfully: " << toHex(enc_err.returnValue());
        else
            PLOG_ERROR << "Failed to encrypt data: " << getErrorMessage(enc_err);

        ErrorStruct<bool> err_write = api.writeToFile();
        if (err_write.isSuccess())
            PLOG_DEBUG << "Wrote to file successfully";
        else
            PLOG_ERROR << "Failed to write to file: " << getErrorMessage(err_write);
    } else {
        enum ACTION { ADD, SELECT, DELETE, UNSELECT, VERPASS, CREATEDH };
        ACTION ACT1 = SELECT;
        ACTION ACT2 = VERPASS;
        ACTION ACT3 = CREATEDH;

        PLOG_INFO << "Starting application";

        API api{FModes::FILEMODE_PASSWORD};
        PLOG_DEBUG << "API created";
        std::filesystem::path dir = api.getEncDirPath().returnValue();
        PLOG_DEBUG << "Getting enc dir path: " << dir;
        ErrorStruct<std::vector<std::string>> paths = api.getAllEncFileNames(dir);
        PLOG_DEBUG << "Getting all enc file names: " << paths.returnValue().size();
        for (auto& path : paths.returnValue()) {
            PLOG_DEBUG << path;
        }

        // INIT
        ErrorStruct<bool> err;
        switch (ACT1) {
            case ADD:
                PLOG_DEBUG << "Adding file";
                err = api.createFile(dir / FILE);
                if (err.isSuccess())
                    PLOG_DEBUG << "Added file successfully";
                else
                    PLOG_WARNING << "Failed to add file: " << getErrorMessage(err);
                PLOG_DEBUG << "Selecting file";
                err = api.selectFile(dir / FILE);
                if (err.isSuccess())
                    PLOG_DEBUG << "Selected file successfully";
                else
                    PLOG_WARNING << "Failed to select file: " << getErrorMessage(err);
                break;

            case SELECT:
                PLOG_DEBUG << "Selecting file";
                err = api.selectFile(dir / FILE);
                if (err.isSuccess())
                    PLOG_DEBUG << "Selected file successfully";
                else
                    PLOG_WARNING << "Failed to select file: " << getErrorMessage(err);
                break;
        }
        // sleep(sleepTime);
        // FILE_SELECTED
        Bytes phash;
        DataHeader dh{HASHMODE_SHA256};
        ErrorStruct<Bytes> temp;

        switch (ACT2) {
            case DELETE:
                PLOG_DEBUG << "Deleting file";
                err = api.deleteFile();
                if (err.isSuccess())
                    PLOG_DEBUG << "Deleted file successfully";
                else
                    PLOG_WARNING << "Failed to delete file: " << getErrorMessage(err);
                break;

            case UNSELECT:
                PLOG_DEBUG << "Unselecting file";
                err = api.unselectFile();
                if (err.isSuccess())
                    PLOG_DEBUG << "Unselected file successfully";
                else
                    PLOG_WARNING << "Failed to unselect file: " << getErrorMessage(err);
                break;

            case VERPASS:
                PLOG_DEBUG << "Verifying password";
                temp = api.verifyPassword(PASS);
                if (temp.isSuccess()) {
                    PLOG_DEBUG << "Verified password successfully";
                    phash = temp.returnValue();
                    PLOG_DEBUG << "Password hash: " << toHex(phash);
                } else
                    PLOG_WARNING << "Failed to verify password: " << getErrorMessage(temp);
                break;

            case CREATEDH:
                PLOG_DEBUG << "Creating data header";
                ErrorStruct<DataHeader> temp = api.createDataHeader(PASS, dsi);
                if (temp.isSuccess()) {
                    PLOG_DEBUG << "Created data header successfully";
                    dh = temp.returnValue();
                    PLOG_DEBUG << "DataHeader: " << dh.getDataHeaderParts() << "\n Hex: " << toHex(dh.getHeaderBytes());
                } else
                    PLOG_WARNING << "Failed to create data header";
                break;
        }

        // DECRYPTED
        ErrorStruct<FileDataStruct> fds = api.getDecryptedData();
        if (fds.isSuccess())
            PLOG_DEBUG << "Got decrypted data successfully: " << toHex(fds.returnValue().dec_data);
        else
            PLOG_ERROR << "Failed to get decrypted data: " << getErrorMessage(fds);
        // PasswordData pd;
        // if(!pd.constructFileData(fds).isSuccess())
        //     PLOG_ERROR << "Failed to construct password data";
        // else
        //     PLOG_DEBUG << "Constructed password data successfully";

        Bytes b;
        b.setBytes(std::vector<unsigned char>{0x00, 0x01, 0x02, 0x03, 0x04});
        fds.returnRef().dec_data = b;
        // change salt
        PLOG_DEBUG << "Changing salt";
        api.changeSalt();
        ErrorStruct<Bytes> enc_err = api.getEncryptedData(fds.returnRef());
        if (enc_err.isSuccess())
            PLOG_DEBUG << "Encrypted data successfully: " << toHex(enc_err.returnValue());
        else
            PLOG_ERROR << "Failed to encrypt data: " << getErrorMessage(enc_err);

        ErrorStruct<bool> err_write = api.writeToFile();
        if (err_write.isSuccess())
            PLOG_DEBUG << "Wrote to file successfully";
        else
            PLOG_ERROR << "Failed to write to file: " << getErrorMessage(err_write);
    }
    // WORK
    timer.stop();
    PLOG_INFO << "Application finished in " << timer.getTime() / 1000.0 << " seconds";
}
