cmake_minimum_required(VERSION 2.6) 
project(imageclipper)

find_package(Boost COMPONENTS system filesystem)
find_package(OpenCV REQUIRED)

add_executable(imageclipper src/imageclipper.cpp)

include_directories(${Boost_INCLUDE_DIR} src)
target_link_libraries(imageclipper ${OpenCV_LIBS} ${Boost_LIBRARIES})

