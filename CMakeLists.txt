cmake_minimum_required(VERSION 3.1)
project(fourier)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENTIONS OFF)

if (CMAKE_COMPILER_IS_GNUCXX)
    add_compile_options(-Wall -Wextra -Werror -pedantic-errors)
    if (WIN32)
        add_definitions(-D_USE_MATH_DEFINES)
    endif (WIN32)
endif (CMAKE_COMPILER_IS_GNUCXX)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

set(HEADERS
    src/commons.h
    src/decompose.h
    src/dft.h
    src/filter.h
    src/generate.h
    src/logger.h
    src/wave.h
)

set(SOURCES
    src/commons.cpp
    src/decompose.cpp
    src/dft.cpp
    src/filter.cpp
    src/generate.cpp
    src/logger.cpp
    src/wave.cpp
    src/main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
