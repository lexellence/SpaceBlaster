cmake_minimum_required(VERSION 3.16) # target_precompile_headers
cmake_policy(SET CMP0076 NEW)

add_subdirectory(../../d2d/repo d2d)

project(space)
add_executable(${PROJECT_NAME})
add_subdirectory(${PROJECT_SOURCE_DIR}/Source)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/Source)
target_link_libraries(${PROJECT_NAME} PUBLIC d2d)

# Precompiled Header
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.16)
    message(STATUS "Compiling using pre-compiled header support")
    target_precompile_headers(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/Source/pch.h)
else()
    message(STATUS "Disabled pre-compiled header support")
endif()

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)


