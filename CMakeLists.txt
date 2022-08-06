cmake_minimum_required(VERSION 3.15)
set(CMAKE_VERBOSE_MAKEFILE OFF)
project(pam_telegram_authenticator CXX)

string(RANDOM LENGTH 19 ALPHABET 0123456789 RANDOM_SEED)
message(STATUS "Random seed: ${RANDOM_SEED}")

set(CMAKE_BUILD_TYPE Release)
string(APPEND CMAKE_C_FLAGS_INIT " -fPIC ${VCPKG_C_FLAGS} ")
string(APPEND CMAKE_CXX_FLAGS_INIT " -fPIC ${VCPKG_CXX_FLAGS}")

add_definitions("-D__RANDOM_SEED__=${RANDOM_SEED}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdata-sections -ffunction-sections -std=gnu++2a")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(test src/test.cpp)
target_link_libraries(test PUBLIC ${CONAN_LIBS})

add_executable(main src/main.cpp)
target_link_libraries(main PUBLIC ${CONAN_LIBS})

add_library(pam_telegram_authenticator SHARED
    src/pam_telegram_authenticator.cpp
    src/lib/PamWrapper/PamWrapper.cpp
    src/lib/TgAuthBot/TgAuthBot.cpp
)
target_include_directories(pam_telegram_authenticator PRIVATE src/lib)
target_link_libraries(pam_telegram_authenticator PUBLIC ${CONAN_LIBS})
