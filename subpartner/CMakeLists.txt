project(subpartner)

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

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h")

ADD_DEFINITIONS(-DUNICODE)
ADD_DEFINITIONS(-D_UNICODE)

add_executable(${PROJECT_NAME} WIN32 
    ${SOURCES}  ${HEADERS} 
)


set_code_sign("clo subprocess partner" ${PROJECT_NAME})


set(ADDITIONAL_LIBRARY_DEPENDENCIES
	"Shlwapi"
	"Credui"
	"Secur32"
  "D:/github/cef-project/third_party/cef/cef_binary_127.3.1+g6cbb30e+chromium-127.0.6533.100_windows64/Debug/libcef.lib"
  "D:/github/cef-project/third_party/cef/cef_binary_127.3.1+g6cbb30e+chromium-127.0.6533.100_windows64/Debug/cef_sandbox.lib"
  "D:/github/cef-project/build/libcef_dll_wrapper/Debug/libcef_dll_wrapper.lib"
)



#conan_target_link_libraries(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} "${ADDITIONAL_LIBRARY_DEPENDENCIES}")
set_target_properties(${PROJECT_NAME} PROPERTIES
 LINK_FLAGS "/ignore:4099"
)

