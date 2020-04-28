cmake_minimum_required(VERSION 3.5)
project(deLIBird C)

set(CMAKE_C_STANDARD 11)

# Include header files
include_directories(include)

# Link custom path
if(DEFINED CMAKE_LIBRARY_PATH)
    include_directories(${CMAKE_LIBRARY_PATH}/include)
    link_directories(${CMAKE_LIBRARY_PATH}/lib)
endif(DEFINED CMAKE_LIBRARY_PATH)

# Find source files
file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "src/*.c")

# Create shared library
add_library(${PROJECT_NAME} SHARED ${SOURCES})

#Importing libraries
target_link_libraries(${PROJECT_NAME} commons)

# Decide where to install (Default is GNU Dir)
if(NOT DEFINED CMAKE_LIBRARY_PATH)
    set(CMAKE_LIBRARY_PATH /usr)
endif(NOT DEFINED CMAKE_LIBRARY_PATH)

# Install library
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_LIBRARY_PATH}/lib)

# Install library headers
install(DIRECTORY include/ DESTINATION ${CMAKE_LIBRARY_PATH}/include/${PROJECT_NAME})