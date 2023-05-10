find_package(OpenSSL REQUIRED)

#executable
add_executable(pman main.cpp bytes.cpp block.cpp rng.cpp pwfunc.cpp filehandler.cpp app.cpp utility.cpp dataHeader.cpp sha256.cpp sha384.cpp sha512.cpp hash_modes.cpp chainhash_modes.cpp)
target_link_libraries(pman ${OPENSSL_LIBRARIES} pthread)
target_include_directories(pman PUBLIC ${INCLUDE_DIR})