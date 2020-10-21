cmake_minimum_required(VERSION 3.2)

project(ld3sdl)

option(WITH_FMOD "Build against fmod/libfmod-3.75.so instead of SDL2_mixer" OFF)

option(FORCE32 "Cross-compile a 32-bit version even on 64-bit machines" OFF)

if("${CMAKE_SYSTEM}" MATCHES "Linux")
	set(TARGET_OS_LINUX ON)
endif()

if(FORCE32)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
endif()

if(TARGET_OS_LINUX)
	if(CMAKE_SIZEOF_VOID_P MATCHES "8" AND NOT(FORCE32))
		set(BIN_LIBROOT "x86_64")
		set(CMAKE_EXECUTABLE_SUFFIX ".x86_64")
		set(BIN_RPATH "\$ORIGIN/x86_64")
	else()
		set(BIN_LIBROOT "x86")
		set(CMAKE_EXECUTABLE_SUFFIX ".x86")
		set(BIN_RPATH "\$ORIGIN/x86")
	endif()


	set(CMAKE_SKIP_BUILD_RPATH TRUE)
	set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
	set(CMAKE_INSTALL_RPATH ${BIN_RPATH})
	set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)

else()
	#TODO: Support non-Linux setups
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIR})

add_executable(ld3
	entity.cpp
	ld3.cpp
	ld3main.cpp
	main.cpp
	winapp.cpp
	#	winproc.cpp
)
target_link_libraries(ld3 ${SDL2_LIBRARY})
if(WITH_FMOD)
	target_link_libraries(ld3 ${CMAKE_SOURCE_DIR}/fmod/libfmod-3.75.so)
else()
	find_package(SDL2_mixer REQUIRED)
	include_directories(${SDL2_MIXER_INCLUDE_DIR})
	target_link_libraries(ld3 ${SDL2_MIXER_LIBRARY})
endif()

