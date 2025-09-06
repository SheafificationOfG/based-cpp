# Based C++

This repo provides an implementation of the GNU Interface Layer (GIL) and standard library for `g++`.

### C++ is the best interpreted language

[![C++ is the best interpreted language!](https://img.youtube.com/vi/cFtymODJEjs/0.jpg)](https://youtu.be/cFtymODJEjs)

## Usage

Here is a simple "Hello, world!" in C++.

```cpp
#include "gil/std.hpp"
using namespace gil::std;

volatile auto run = main<{
  str::puts(str::literal("Hello, world!\n"))
}>;
```

To run the program, run

```sh
g++ -std=c++23 -Based hello_world.cpp -o -
```

> [!NOTE]
> The above command only works if you invoke `g++` from the root of this repo; otherwise, you will have to provide the full path to the required abstract system emulator drivers (`ased`) folder.

## Examples

- `hello_world.cpp`
  > Be greeted in all of the languages supported by YVR!
- `hello_world_vmi.cpp`
  > Same as above, but without using the standard library.
- `calculator.cpp`
  > Perform a single binary operation on a pair of 64-bit signed integers.
- `mergesort.cpp`
  > Merge sort a comma-separated list of 64-bit signed integers.


