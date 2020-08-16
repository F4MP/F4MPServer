cmake_minimum_required(VERSION 3.7)
project(F4MPServer)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} CMakeModules/)

set(executable F4MPServer)
set(SrcDIR ./src)
set(IncludeDIR ./include)

# Setting resource path
add_definitions(-D_RESOURCES="${CMAKE_SOURCE_DIR}/resources/")
message(${CMAKE_SOURCE_DIR}/resources)

# Project definitions

if (UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif(UNIX)

# Include Dependencies

set(THREADS_PREFER_PTHREAD_FLAD ON)
find_package(Threads REQUIRED)

if (WIN32)
	# set(CMAKE_CXX_FLAGS "/Ox")
	include_directories(${executable}
	)
endif (WIN32)

if (UNIX)
	include_directories(${executable}
	)
endif (UNIX)

include_directories(${executable}
	${IncludeDIR}
)

# Building

file(GLOB SourceFiles 
	${SrcDIR}/*
	${SrcDIR}/ThirdParty/*
	${SrcDIR}/ThirdParty/nlohmann*
)

add_executable(${executable} ${SourceFiles})

set_target_properties(${executable} PROPERTIES
	CXX_STANDARD 17
	CXX_EXTENSIONS ON
)


# Link Dependencies

if (UNIX)
	target_link_libraries(${executable}
	)
endif (UNIX)

if (WIN32)
	target_link_libraries(${executable}
	)

endif (WIN32)

target_link_libraries(${executable}
	${CMAKE_DL_LIBS}
	${CMAKE_SOURCE_DIR}/lib/*
)
