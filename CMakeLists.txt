cmake_minimum_required(VERSION 3.25)
#cmake_policy(SET CMP0091 NEW)
project(sciter-cp VERSION 6.7.0)

find_package(OPENSSL REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(jwt-cpp REQUIRED)

#include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
#conan_basic_setup()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /ignore:4099")


set(SIGN_PARAMS /sha1 "4f29aa91faa9a6d504b5458070c9802d36e6c137" /fd SHA256 /t http://timestamp.sectigo.com /du "http://www.deepnetsecurity.com")

macro (set_code_sign description target)
    add_custom_command( TARGET ${target}
        POST_BUILD
        COMMAND signtool sign ${SIGN_PARAMS} /d ${description} $<TARGET_FILE:${target}>
        VERBATIM
    )
endmacro (set_code_sign)


add_subdirectory(samplewrapexistingcredentialprovider)
add_subdirectory(subpartner)
