find_package(OpenSSL REQUIRED)

#executable
add_executable(pman main.cpp 
    bytes.cpp 
    block.cpp block_decrypt.cpp block_encrypt.cpp 
    blockchain.cpp blockchain_stream.cpp blockchain_stream_decrypt.cpp blockchain_stream_encrypt.cpp blockchain_decrypt.cpp blockchain_encrypt.cpp 
    api.cpp rng.cpp pwfunc.cpp format.cpp chainhash_data.cpp filehandler.cpp file_modes.cpp utility.cpp 
    dataheader.cpp sha256.cpp sha384.cpp sha512.cpp hash_modes.cpp chainhash_modes.cpp timer.cpp)
target_link_libraries(pman ${OPENSSL_LIBRARIES} pthread)
target_include_directories(pman PUBLIC ${INCLUDE_DIR})