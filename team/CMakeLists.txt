cmake_minimum_required(VERSION 3.5)
project(team C)

set(CMAKE_C_STANDARD 11)

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug" OR "${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG")
    add_definitions(
            -DTEAM_DEVELOPMENT_MODE
            -DINTERNAL_LOG_LEVEL=LOG_LEVEL_DEBUG
            -DSHOW_INTERNAL_CONSOLE=1
            -DCORRER_TESTS=1
            -DESPERAR_OBJETIVO_GLOBAL=0
            -DACTIVAR_RETARDO_CPU=0
    )
endif()

# .h files
include_directories(include)
include_directories(/usr/include/delibird) # delibird is our shared lib

if(DEFINED CMAKE_LIBRARY_PATH)
    include_directories(${CMAKE_LIBRARY_PATH}/include)
    link_directories(${CMAKE_LIBRARY_PATH}/lib)
endif(DEFINED CMAKE_LIBRARY_PATH)

# .c files
file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "src/*.c")
# sin esto no vivimos
add_executable(${PROJECT_NAME} ${SOURCES})

# Commons y otras librerias
target_link_libraries(${PROJECT_NAME} commons delibird pthread)

# Config
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/config/ $<TARGET_FILE_DIR:${PROJECT_NAME}>/config)

# Logs
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory logs)