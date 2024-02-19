# WebProc

A utility for running a screen-like program in daemon mode and allow monitoring of stdin/stdout over an HTTP graphical interface. The program maintains two threads: one to run the program itself and one to interface with stdin/stdout of the program. WebProc can also configure tasks to run on a schedule.

This program was initially designed for interfacing with a Minecraft server.

_Note: This program is currently under development, all desired features have not been implemented_

## Dependencies

* cmake, version 3.22 or newer
* C++ build tools

## Build Instructions

Create a `build` directory, and configure it as the binary directory for the build system:

```
mkdir build
cd build
cmake ..
```

Then, build the executable:

```
make
```

Install the executable in a system path:

_Note: The location of this executable may be changed by providing the `-DCMAKE_INSTALL_PREFIX` argument to the `cmake` command in the first step._

```
sudo make install
```
