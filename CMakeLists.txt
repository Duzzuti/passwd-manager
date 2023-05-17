cmake_minimum_required(VERSION 3.0.0)
project(passwd-manager VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)

###############################
# Constants
###############################

################################
# GTest
################################

if(CMAKE_BUILD_TYPE STREQUAL "Debug")

  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  )
  # For Windows: Prevent overriding the parent project's compiler/linker settings
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)
endif()

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


if(NOT WIN32)
  install(TARGETS pman DESTINATION /usr/local/bin)
else()
  #win32
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_subdirectory(${TEST_DIR})

endif()