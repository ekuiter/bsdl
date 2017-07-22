## Build instructions

- place Boost in lib/boost folder, compile all its required static libraries (see below)

### Mac OS

- use lib/.../create_dylib to create the correct library files (download a fresh copy of the source code).
- curl and curses are pre-installed on Mac OS, the others are copied into the bin_macos folder by CMake.

### Windows

- Install CMake, add C:\Program Files\CMake\bin to PATH.
- Install MinGW. (mingw-developer-toolkit, mingw32-base, mingw32-gcc-g++@4.9.3, msys-base, both mingw32-libpdcurses)
- Start MSYS, goto root folder.
- If all libraries are set up ==> cmake -G "MSYS Makefiles".
- compile the following libs in a fresh directory (no cmake cache etc.)!
- compile Boost with (in cmd.exe with C:\MinGW\bin in PATH):
```
bootstrap gcc
b2 toolset=gcc --with-filesystem --with-system
```
- compile curl with:
```
./configure --with-winssl
make
make install
```
- compile curlcpp with:
```
change srcCMakeLists.txt line 17 from "-std=c++11" to "-std=gnu++11"
cd build
cmake -G "MSYS Makefiles" -DBUILD_SHARED_LIBS=1 ..
```
- copy src/libcurlcpp.dll/.dll.a to curlcpp root folder (beside .dylib)
- compile gumbo-parser with:
```
./autogen.sh
make
```
- copy .dll/.dll.a to gumbo-parser root folder
- compile gumbo-query with:
```
cd build
cmake -G "MSYS Makefiles" -DGumbo_INCLUDE_DIR=../../gumbo-parser/src -DGumbo_LIBRARY=../../gumbo-parser/libgumbo.dll.a -DGumbo_static_LIBRARY= ..
```
- copy .dll.a from lib and .dll from build/src to gumbo-query root folder
- (mind the changes in src/Selection.h!)
