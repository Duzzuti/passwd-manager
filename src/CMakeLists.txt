find_package(OpenSSL REQUIRED)

#executable
add_executable(passwd_manager main.cpp bytes.cpp block.cpp rng.cpp pwfunc.cpp)#blockchain.cpp
target_link_libraries(passwd_manager ${OPENSSL_LIBRARIES} pthread)
target_include_directories(passwd_manager PUBLIC ${INCLUDE_DIR})
