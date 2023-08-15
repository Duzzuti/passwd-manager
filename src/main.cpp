#include <iostream>

#include "api.h"
#include "logger.h"

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
    std::filesystem::path FILE = "test.enc";
    std::string PASS = "test";
    int sleepTime = 5;

    DataHeaderSettingsIters dsi;
    {
        dsi.setChainHash1Mode(CHAINHASH_CONSTANT_COUNT_SALT);
        dsi.setChainHash2Mode(CHAINHASH_QUADRATIC);
        dsi.setChainHash1Iters(100000);
        dsi.setChainHash2Iters(2000);
        dsi.setFileDataMode(FILEMODE_PASSWORD);
        dsi.setHashMode(HASHMODE_SHA256);
    }

    enum ACTION { ADD, SELECT, DELETE, UNSELECT, VERPASS, CREATEDH };
    ACTION ACT1 = ADD;
    ACTION ACT2 = DELETE;

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
    sleep(sleepTime);
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
                PLOG_WARNING << "Failed to verify password";
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

    // WORK

    PLOG_INFO << "Application finished";
}
