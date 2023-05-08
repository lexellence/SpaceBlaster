cmake_minimum_required(VERSION 3.11)    # GoogleTest

project(space-d2d)

include_directories(${PROJECT_SOURCE_DIR}/Source) 
add_subdirectory(../../d2d/repo d2d) 
add_subdirectory(Source space) 
