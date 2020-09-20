cmake_minimum_required(VERSION 3.2)

project(slx_win32)


SET(LINUXLIB_PATH
    "/home/david/builds/libs"
    CACHE PATH "Location of private builds of linux libraries"
)

link_directories(
    ${LINUXLIB_PATH}/libs-32
)
include_directories(
    ${LINUXLIB_PATH}/include
    ${LINUXLIB_PATH}/include/SDL2
)

set(BIN_LIBROOT "x86")
set(BIN_SUFFIX ".x86")
set(BIN_RPATH "\$ORIGIN")

set(CMAKE_SKIP_BUILD_RPATH TRUE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH ${BIN_RPATH})
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


add_executable(ld3
	entity.cpp
	ld3.cpp
	ld3main.cpp
	main.cpp
	winapp.cpp
	#	winproc.cpp
)
if(DEFINED WITH_FMOD)
	target_link_libraries(ld3 ${CMAKE_SOURCE_DIR}/fmod/libfmod-3.75.so)
else()
	target_link_libraries(ld3 SDL2_mixer)
endif()
target_link_libraries(ld3 SDL2)
