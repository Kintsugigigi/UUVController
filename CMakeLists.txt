cmake_minimum_required(VERSION 3.18)

PROJECT(USVController)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
#add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
#add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
#add_subdirectory()
file(GLOB SRC_FILES
        # "${PROJECT_SOURCE_DIR}/main.cpp"
        "${PROJECT_SOURCE_DIR}/Source/*.h"
        # "${PROJECT_SOURCE_DIR}/Source/*.cpp"
        "${PROJECT_SOURCE_DIR}/Source/Tools/*"
        "${PROJECT_SOURCE_DIR}/Source/Action/*"
        "${PROJECT_SOURCE_DIR}/Source/Condition/*"
        "${PROJECT_SOURCE_DIR}/Source/Communication/*"
        "${PROJECT_SOURCE_DIR}/Source/Control/*"
        "${PROJECT_SOURCE_DIR}/Source/Navigation/*"
        "${PROJECT_SOURCE_DIR}/Source/Postman/*"
        "${PROJECT_SOURCE_DIR}/Source/Sensor/*"
        "${PROJECT_SOURCE_DIR}/Source/UUVTest/*")

include_directories("./Include/")

add_executable(${CMAKE_PROJECT_NAME} ${SRC_FILES})

#include_directories("./include/")

target_include_directories(USVController PUBLIC Include)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

    target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE  fmt)
endif()

#target_sources()

#add_subdirectory()
#target_compile_features(${CMAKE_PROJECT_NAME} PRIVATE cxx_std_20)



