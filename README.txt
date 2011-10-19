To build Andromeda, make sure that Qt is installed on your system.
You can download Qt from http://qt.nokia.com/downloads
On Linux, you can also install it using package manager.

Also you need to install CMake - http://www.cmake.org/cmake/resources/software.html

After preparations, open terminal, cd to andromeda source folder and run following commands:

$ mkdir build
$ cd build
$ cmake ../
$ make

Note: on Windows, instead of running make, you have to run "mingw32-make" (if using MinGW)
or "nmake" (if using VS)

On Linux, after that, run:

$ make install
