find_package(OpenSSL REQUIRED)

#executable
add_executable(passwd_manager main.cpp bytes.cpp block.cpp)
target_include_directories(passwd_manager PUBLIC ${INCLUDE_DIR})
