Image Clipper
-------------

This is a fork of the [imageclipper](https://github.com/JoakimSoderberg/imageclipper)
program initially written by Naotoshi Seo.

This fork was made initially for myself to improve its capabilities. 


From the original description:

> It is often required to crop images manually fast for computer vision researchers to gather training and testing image sets, e.g., cropping faces in images.
>
> This simple multi-platform (Windows, OSX and Linux were verified) software helps you to clip images manually fast.

Addition:
> Add some actions which similar to Photoshop's actions, such Ctrl & Draw to draw a square rather than a rectangle, Alt & Draw to draw a rectangle from the center, and so on.
>
> Fixed the capture size
>
> Friendly UI and operations

## Building

To build you need [CMake](http://www.cmake.org/), [OpenCV](http://opencv.org/) and [Boost](http://www.boost.org/)
on your system

### Linux & OSX

Install requirements using your favorite package system or download the sources yourself.

```bash
$ sudo apt-get install cmake libopencv-dev libboost-all-dev
```

Using [MacPorts](http://www.macports.org/).

```bash
$ sudo port install cmake boost opencv
```

And then build:

```bash
$ git clone <repo url>
$ cd imageclipper
$ mkdir build && cd build
$ cmake ..                                     # This should work in most cases.

# (Optional) Or specifying your own builds of OpenCV or Boost.
$ cmake -DOpenCV_DIR=/path/to/opencv/build/ -DBOOST_ROOT=/path/to/boost ..

$ cmake --build .                              # Builds the program.
```

### Windows

On windows you need to download [Visual Studio for Windows Desktop](http://www.visualstudio.com/), [CMake](http://www.cmake.org/cmake/resources/software.html) and [git](http://git-scm.com/).

###### Boost

Has been tested with this verison on Windows 7, but should work with whatever you like:
[Boost 1.55.0 32-bit MSVC12](http://sourceforge.net/projects/boost/files/boost-binaries/1.55.0-build2/)
The default unpack location is **c:\local\boost_1_55_0**, if you change it you need to know the path when building.

###### OpenCV

Download and unpack [OpenCV](http://opencv.org/downloads.html) and remember the path.

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
