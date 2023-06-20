cmake_minimum_required(VERSION 3.0.0)
include(FetchContent)
project(passwd-manager VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)

# Add the './cmake' folder so we can use the Find<Library>.cmake file
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
message(STATUS "CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")

###############################
# Constants
###############################

################################
# GTest
################################

if(CMAKE_BUILD_TYPE STREQUAL "Debug")

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

# --- Fetch FTXUI --------------------------------------------------------------
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG v4.1.1
  GIT_PROGRESS TRUE
  GIT_SHALLOW FALSE
)

FetchContent_GetProperties(ftxui)
if(NOT ftxui_POPULATED)
  FetchContent_Populate(ftxui)
  add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()
 
# -----------------------------------------------------------------------------
# --- Fetch Plog --------------------------------------------------------------
FetchContent_Declare(
  plog
  GIT_REPOSITORY https://github.com/SergiusTheBest/plog
  GIT_TAG 1.1.9
)
FetchContent_MakeAvailable(plog)
# add to includes
include_directories(${plog_SOURCE_DIR}/include)
# -----------------------------------------------------------------------------

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

