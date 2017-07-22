## Build instructions

- place Boost in lib/boost folder, compile all its required static libraries (see below)

### Mac OS

- use lib/.../create_dylib to create the correct library files (download a fresh copy of the source code).
- curl and curses are pre-installed on Mac OS, the others are copied
into the bin_macos folder by CMake.
- cmake . (compile with clang, LLVM 8.1)

### Windows

- Install CMake, add C:\Program Files\CMake\bin to PATH.
- Install MinGW. (mingw-developer-toolkit, mingw32-base, mingw32-gcc-g++@4.9.3, msys-base, both mingw32-libpdcurses)
- Start MSYS, goto root folder.
- If all libraries are set up ==> cmake -G "MSYS Makefiles" .
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

### Linux (tested on openSUSE)

- Install some dependencies: sudo zypper install cmake gcc6-c++ libtool doxygen libcurl-devel ncurses-devel
- compile libraries:
- boost: ./bootstrap.sh gcc && ./b2 toolset=gcc link=static
  --with-filesystem --with-system, then copy to stage/lib/libboost_*_linux.a
- curlcpp: cmake -DBUILD_SHARED_LIBS=1 . && make, then copy src/libcurlcpp.so beside .dylib
- gumbo-parser: /autogen.sh && ./configure && make, then copy .libs/libgumbo.so.1 beside .dylib
- gumbo-query: cd build && cmake -DGumbo_INCLUDE_DIR=../../gumbo-parser/src -DGumbo_LIBRARY=../../gumbo-parser/libgumbo.so -DGumbo_static_LIBRARY= ..
  then copy lib/libgq.so beside .dylib
- finally, run: CXX=g++-6 cmake .
