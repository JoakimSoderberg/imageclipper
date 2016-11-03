cmake_minimum_required(VERSION 2.6) 
project(imageclipper)

option(WITH_TBB "Turn on support for TBB, Threading Building Blocks. You must have an OpenCV compiled with support for this" OFF)

if (MSVC)
	# We link statically on windows so we don't have to copy DLLs around.
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	set(OpenCV_STATIC ON)

	set(BOOST_INCLUDEDIR "${BOOST_ROOT}/boost")
	set(BOOST_LIBRARYDIR "${BOOST_ROOT}/lib32-msvc-12.0")
	set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "${BOOST_INCLUDEDIR}")
	set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${BOOST_LIBRARYDIR}")
	set(Boost_USE_STATIC_LIBS ON)
	set(Boost_USE_STATIC_RUNTIME ON)
	set(Boost_DETAILED_FAILURE_MSG ON)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

if (WITH_TBB)
	find_package(TBB REQUIRED)
endif()

find_package(Boost COMPONENTS system filesystem)
find_package(OpenCV REQUIRED)

add_executable(imageclipper src/imageclipper.cpp)

include_directories(${Boost_INCLUDE_DIR} ${OpenCV_INCLUDE_DIRS} src)
link_directories(${Boost_LIBRARY_DIR})
target_link_libraries(imageclipper ${OpenCV_LIBS} ${Boost_LIBRARIES})

if (WITH_TBB)
	include_directories(${TBB_INCLUDE_DIRS})
	link_directories(${TBB_INCLUDE_DIRS})
	target_link_libraries(imageclipper ${TBB_LIBRARIES})
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

message("OpenCV Libs: ${OpenCV_LIBS}")
message("Boost libs: ${Boost_LIBRARIES}")
message("Boost link dir: ${Boost_LIBRARY_DIR}")

