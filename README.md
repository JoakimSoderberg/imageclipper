Image Clipper
-------------

## Introduction

This is a fork of the [imageclipper](https://code.google.com/p/imageclipper/)
program initially written by Naotoshi Seo.

## Requirements

To build [OpenCV](http://opencv.org/) and [Boost](http://www.boost.org/)
needs to be installed on the system:

### OSX:

Using [MacPorts](http://www.macports.org/)

```bash
$ sudo port install boost
$ sudo port install opencv
```

### Windows

On windows you need to download [Visual Studio for Windows Desktop](http://www.visualstudio.com/)
and of course [git](http://git-scm.com/).

Download and unpack the [Boost binaries](http://www.boost.org/users/download/)
and [OpenCV](opencv.org/downloads.html) in known directories.

## Building

The original Makefile used hard coded paths and did not work on OSX
so this new project uses [CMake](http://cmake.org/) for a cross platform
build solution.

### OSX & Linux:

```bash
$ git clone <repo url>
$ cd imageclipper
$ mkdir build && cd build
$ cmake ..
$ cmake -DOpenCV_DIR=/path/to/opencv/build/ .. # Or if you downloaded OpenCV from http://opencv.org/
$ cmake --build .
```

### Windows

From the **git bash** console:
(use cmd.exe if you prefer)

```bash
$ git clone <repo url>
$ cd imageclipper
$ mkdir build && cd build
$ cmake -DOpenCV_DIR=/path/to/opencv/build/ -DBoost_DIR=/path/to/boost/dir ..
$ cmake --build .
$ start imageclipper.sln # If you want to open in Visual Studio instead.
```