# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

set(projectName "gray-scott-cellular")

project(${projectName})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
add_subdirectory(main)
message(STATUS "🟢 main/CMakeLists.txt loaded!")
