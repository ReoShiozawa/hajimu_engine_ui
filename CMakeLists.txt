cmake_minimum_required(VERSION 3.20)
project(engine_ui C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(HAJIMU_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/../../jp/include")
message(STATUS "HAJIMU_INCLUDE_DIR = ${HAJIMU_INCLUDE_DIR}")

add_library(engine_ui SHARED
    src/eng_ui.c
    src/plugin.c
)

target_include_directories(engine_ui PRIVATE
    ${HAJIMU_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)

if(UNIX AND NOT APPLE)
    target_link_libraries(engine_ui PRIVATE m)
endif()

set_target_properties(engine_ui PROPERTIES
    OUTPUT_NAME "engine_ui"
    SUFFIX ".hjp"
    PREFIX ""
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build"
)
