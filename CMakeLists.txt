cmake_minimum_required(VERSION 3.15)

project(pam_telegram_authenticator LANGUAGES CXX VERSION 0.0.1 DESCRIPTION "PAM to authenticate via Telegeram")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_BUILD_TYPE Release)

# automatically on when building shared lib
string(APPEND CMAKE_C_FLAGS_INIT " -fPIC ${VCPKG_C_FLAGS} ")
string(APPEND CMAKE_CXX_FLAGS_INIT " -fPIC ${VCPKG_CXX_FLAGS}")

string(RANDOM LENGTH 19 ALPHABET 0123456789 RANDOM_SEED)
message(STATUS "Random seed: ${RANDOM_SEED}")
add_definitions("-D__RANDOM_SEED__=${RANDOM_SEED}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdata-sections -ffunction-sections -std=gnu++2a")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

#add_executable(test src/test.cpp)
#target_link_libraries(test PUBLIC ${CONAN_LIBS})
#
#add_executable(main src/main.cpp)
#target_link_libraries(main PUBLIC ${CONAN_LIBS})

add_library(${PROJECT_NAME} SHARED
        ${CMAKE_CURRENT_SOURCE_DIR}/src/pam_telegram_authenticator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/PamWrapper/PamWrapper.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/TgAuthBot/TgAuthBot.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/Utils.h
        )
#set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/lib
        ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/PamWrapper/
        )
target_link_libraries(${PROJECT_NAME} PUBLIC ${CONAN_LIBS})

# For access to standard installation directory variables (CMAKE_INSTALL_xDIR).
include(GNUInstallDirs)
# Set library shared object and API header file to install.
install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION /lib/security)
