project(samplewrapexistingcredentialprovider)

set(ModuleName samplewrapexistingcredentialprovider)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(/wd4267)

add_compile_definitions(DASAUTHEXTENSION_EXPORTS)
add_compile_definitions(JWT_CLAIM_EXPLICIT=0)

#Generate pdb files for release build
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT /Zi")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF" CACHE STRING "" FORCE)

include_directories( E:/oslib/sciter-js-sdk-main/include )
include_directories( ../Helpers )


file(GLOB HELP_SOURCES "../Helpers/*.cpp")
file(GLOB HELP_HEADERS "../Helpers*.h")

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h")

ADD_DEFINITIONS(-DUNICODE)
ADD_DEFINITIONS(-D_UNICODE)
# stdafx.cpp is precompiled file, skip it
#list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/stdafx.cpp")
#ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" SOURCES)



#ADD_CUSTOM_COMMAND(
#    OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/${MESSAGE_SOURCE}.h" "${CMAKE_CURRENT_SOURCE_DIR}/${MESSAGE_SOURCE}.rc"
#    COMMAND  mc.exe /A "${CMAKE_CURRENT_SOURCE_DIR}/${MESSAGE_SOURCE}.mc" /h  "${CMAKE_CURRENT_SOURCE_DIR}"
#        /r "${CMAKE_CURRENT_SOURCE_DIR}"
#    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${MESSAGE_SOURCE}.mc"
#)


add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/sciter_resources.h
  COMMAND E:/oslib/sciter-js-sdk-main/bin/windows/packfolder.exe  ${CMAKE_CURRENT_SOURCE_DIR}/ui sciter_resources.h -v "sciter_resources"
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/ui
  COMMENT "Generating sciter_resources.h"
  )

# Custom target to ensure the custom command is executed
add_custom_target(generate_resources ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/sciter_resources.h)



add_compile_options(-bigobj)

add_library(${ModuleName} SHARED 
    ${SOURCES}  ${HEADERS} ${HELP_SOURCES}  ${HELP_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/sciter_resources.h
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
)



conan_target_link_libraries(${ModuleName})
target_link_libraries(${ModuleName} "${ADDITIONAL_LIBRARY_DEPENDENCIES}")
set_target_properties(${ModuleName} PROPERTIES
 LINK_FLAGS "/ignore:4099"
)

