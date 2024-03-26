cmake_minimum_required(VERSION 2.8)

project( visualize )
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
add_executable( visualize visualize.cpp )
target_link_libraries( visualize ${OpenCV_LIBS} )