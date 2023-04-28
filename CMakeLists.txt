cmake_minimum_required(VERSION 3.0.0)
project(passwd-manager VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)
################################
# GTest
################################

# include(ExternalProject)
# #Download and build gtest
# ExternalProject_Add(
#     gtest
#     GIT_REPOSITORY https://github.com/google/googletest.git
#     GIT_TAG v1.13.0
#     CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/gtest-install
# )

# Include gtest as a subdirectory
# add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
#                  ${CMAKE_BINARY_DIR}/gtest-build
#                  EXCLUDE_FROM_ALL)

# # Link gtest to your project
# target_link_libraries(password-manager gtest_main)




# configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
# execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
#         RESULT_VARIABLE result
#         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
# if(result)
#     message(FATAL_ERROR "CMake step for googletest failed: ${result}")
# endif()
# execute_process(COMMAND ${CMAKE_COMMAND} --build .
#         RESULT_VARIABLE result
#         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
# if(result)
#     message(FATAL_ERROR "Build step for googletest failed: ${result}")
# endif()

# # Prevent overriding the parent project's compiler/linker
# # settings on Windows
# set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# # Add googletest directly to our build. This defines
# # the gtest and gtest_main targets.
# add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
#         ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
#         EXCLUDE_FROM_ALL)

# # The gtest/gtest_main targets carry header search path
# # dependencies automatically when using CMake 2.8.11 or
# # later. Otherwise we have to add them here ourselves.
# if (CMAKE_VERSION VERSION_LESS 2.8.11)
#     include_directories("${gtest_SOURCE_DIR}/include")
# endif()




include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


#find_package(GTest REQUIRED)
find_package(OpenSSL REQUIRED)
enable_testing()

set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(TEST_DIR ${PROJECT_SOURCE_DIR}/tests)
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

add_subdirectory(${SRC_DIR})
add_subdirectory(${TEST_DIR})


