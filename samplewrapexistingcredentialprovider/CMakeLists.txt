project(samplewrapexistingcredentialprovider)

find_package(OPENSSL REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(jwt-cpp REQUIRED)
find_package(httplib REQUIRED)
find_package(quill REQUIRED)

set(CONAN_LIBS nlohmann_json::nlohmann_json openssl::openssl jwt-cpp::jwt-cpp httplib::httplib quill::quill)



set(ModuleName samplewrapexistingcredentialprovider)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(/wd4267)

add_compile_definitions(DASAUTHEXTENSION_EXPORTS)
add_compile_definitions(JWT_CLAIM_EXPLICIT=0)
add_compile_definitions(_HAS_ITERATOR_DEBUGGING=0)
#Generate pdb files for release build
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT /Zi")
#set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

include_directories("D:/github/cef-project/third_party/cef/cef_binary_127.3.1+g6cbb30e+chromium-127.0.6533.100_windows64")
include_directories( ../Helpers )


file(GLOB HELP_SOURCES "../Helpers/*.cpp")
file(GLOB HELP_HEADERS "../Helpers*.h")

file(GLOB CEFPART_SOURCES "../cefpart/*.cc")
file(GLOB CEFPART_HEADERS "../cefpart*.h")

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h")

ADD_DEFINITIONS(-DUNICODE)
ADD_DEFINITIONS(-D_UNICODE)

# stdafx.cpp is precompiled file, skip it
#list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/stdafx.cpp")
#ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" SOURCES)



add_compile_options(-bigobj)

add_library(${ModuleName} SHARED 
    ${SOURCES}  ${HEADERS} ${HELP_SOURCES}  ${HELP_HEADERS} ${CEFPART_SOURCES}  ${CEFPART_HEADERS}
    "SampleWrapExistingCredentialProvider.def"
    "resources.rc"
)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")

set_code_sign("sciter in credential provider" ${ModuleName})

#target_precompile_headers(${ModuleName}
#  PRIVATE
#  "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h>"
#)


set(ADDITIONAL_LIBRARY_DEPENDENCIES
	"Shlwapi"
	"Credui"
	"Secur32"
  "D:/github/cef-project/third_party/cef/cef_binary_127.3.1+g6cbb30e+chromium-127.0.6533.100_windows64/Debug/libcef.lib"
  "D:/github/cef-project/third_party/cef/cef_binary_127.3.1+g6cbb30e+chromium-127.0.6533.100_windows64/Debug/cef_sandbox.lib"
  "D:/github/cef-project/build/libcef_dll_wrapper/Debug/libcef_dll_wrapper.lib"
)



#conan_target_link_libraries(${ModuleName})
target_link_libraries(${ModuleName} ${CONAN_LIBS} "${ADDITIONAL_LIBRARY_DEPENDENCIES}")
set_target_properties(${ModuleName} PROPERTIES
 LINK_FLAGS "/ignore:4099"
)

