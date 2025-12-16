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

Note that the program won't execute until all `stdin` is read, which means you need to pass it an `EOF` (e.g., `^D` from `stdin`).

> [!NOTE]
> Use `-O2` or `-O3` if you want things to run faster.
> (I didn't do this in my video... oops).

### System

I've only tested this thing on my machines (Ubuntu 22.04 / 24.04) using `g++ 14.2.0`.
If it doesn't work on your machine, maybe that's actually a good thing.
> See also [#8](https://github.com/SheafificationOfG/based-cpp/issues/8).

## Examples

- `hello_world.cpp`
  > Be greeted in all of the languages supported by YVR!
- `hello_world_vmi.cpp`
  > Same as above, but without using the standard library.
- `calculator.cpp`
  > Perform a single binary operation on a pair of 64-bit signed integers.
- `mergesort.cpp`
  > Merge sort a comma-separated list of 64-bit signed integers.
