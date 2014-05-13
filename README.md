Image Clipper
-------------

## Introduction

This is a fork of the [imageclipper](https://code.google.com/p/imageclipper/)
program initially written by Naotoshi Seo.

## Requirements

To build you need [CMake](http://www.cmake.org/), [OpenCV](http://opencv.org/) and [Boost](http://www.boost.org/)
on your system.

### Linux

On debian.

```bash
$ sudo apt-get install cmake libopencv-dev libboost-dev
```

### OSX

Using [MacPorts](http://www.macports.org/).

```bash
$ sudo port install cmake boost opencv
```

### Windows

On windows you need to download [Visual Studio for Windows Desktop](http://www.visualstudio.com/)
, [CMake](http://www.cmake.org/cmake/resources/software.html) and [git](http://git-scm.com/).

#### Boost
Download and unpack the 32-bit [Boost binaries](http://www.boost.org/users/download/)

Tested with [Boost 1.55.0 32-bit MSVC12](http://sourceforge.net/projects/boost/files/boost-binaries/1.55.0-build2/)

Default location is **c:\local\boost_1_55_0**. You need to know this when building.

#### OpenCV
Download and unpack [OpenCV](http://opencv.org/downloads.html) and remember the path.

## Building

The original Makefile used hard coded paths and did not work on OSX
so this new project uses [CMake](http://cmake.org/) for a cross platform
build solution.

### OSX & Linux

```bash
$ git clone <repo url>
$ cd imageclipper
$ mkdir build && cd build
$ cmake ..                                     # This should work in most cases.

$ cmake -DOpenCV_DIR=/path/to/opencv/build/ .. # Or if you downloaded OpenCV from http://opencv.org/
        -DBOOST_ROOT=/path/to/boost            # If you're using your own Boost build.

$ cmake --build .                              # Builds the program.
```

### Windows

From the **git bash** console:
(use cmd.exe if you prefer)

```bash
$ git clone <repo url>
$ cd imageclipper
$ mkdir build && cd build
$ cmake -DOpenCV_DIR=/path/to/opencv/build/ -DBOOST_ROOT=/c/local/boost_1_55_0/ ..
$ cmake --build . --config Release 
$ start imageclipper.sln # If you want to open in Visual Studio instead.
```

The executable can then be found under **build/bin/Release/imageclipper.exe**
