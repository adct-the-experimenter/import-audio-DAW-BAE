
cmake_minimum_required(VERSION 3.15)

set (CMAKE_CXX_STANDARD 11)

set(PROJECT_NAME importDAWBAE)
project (${PROJECT_NAME} LANGUAGES C CXX)

set(PLUGIN_DIR "${CMAKE_CURRENT_BINARY_DIR}/import-DAW-BAE.lv2")
set(TTL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ttl")
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

find_package(PkgConfig REQUIRED)

pkg_check_modules(LV2 lv2 REQUIRED)
include_directories( ${LV2_INCLUDE_DIRS}  )

pkg_check_modules(SNDFILE sndfile REQUIRED)
include_directories( ${SNDFILE_INCLUDE_DIRS}  )
link_directories   ( ${SNDFILE_LIBRARY_DIRS}  )

file(MAKE_DIRECTORY ${PLUGIN_DIR})

#-DCMAKE_BUILD_TYPE=Release
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -I. -O3 -ffast-math -Wall -Wextra -fPIC -DPIC -fPIE")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -I. -O3 -ffast-math -Wall -Wextra -fPIC -DPIC -fPIE")

set(CMAKE_SHARED_LINKER_FLAGS "-fPIC -shared -Wl,-z,nodelete -Wl,--no-undefined")
#set(CMAKE_EXE_LINKER_FLAGS "${LDFLAGS} -shared -lm -fPIC -DPIC")

#where to find library files .so 
link_directories( /usr/lib /usr/local/lib ) 

set ( PROJECT_INCLUDE_DIR "./" ${SNDFILE_INCLUDE_DIRS})

#for where to find header files for source and library
include_directories(/usr/include /usr/local/include ./include ${PROJECT_INCLUDE_DIR})


set(SOURCES ${SRC_DIR}/importDAW.cpp)

#get_cmake_property(_variableNames VARIABLES)
#list (SORT _variableNames)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()
set ( PROJECT_LINK_LIBS ${SNDFILE_LIBRARY_DIRS})

add_library(${PROJECT_NAME} SHARED ${SOURCES} )


#link libraries
target_link_libraries( ${PROJECT_NAME} ${SNDFILE_LIBRARIES} )

#remove lib part of .so file that cmake prepends
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

#copy importDAW.ttl
add_custom_command(
        TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${TTL_DIR}/importDAW.ttl
                ${PLUGIN_DIR}/importDAW.ttl)

#copy manifest.ttl.in
add_custom_command(
        TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${TTL_DIR}/manifest.ttl
                ${PLUGIN_DIR}/)

#copy .so file
add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                importDAWBAE.so
                ${PLUGIN_DIR}/)

#copy settings.txt
add_custom_command(
        TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/settings.txt
                ${PLUGIN_DIR}/)

#install( DIRECTORY ${PLUGIN_DIR} DESTINATION "${HOME}/.lv2" )
